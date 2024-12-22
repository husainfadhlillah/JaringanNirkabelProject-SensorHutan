// ForestEnvironment.h
#ifndef FOREST_ENVIRONMENT_H
#define FOREST_ENVIRONMENT_H

#include <cmath>
#include <random>

class ForestEnvironment {
private:
    // Baseline values
    double baseTemp;     // Baseline temperature
    double baseHumidity; // Baseline humidity

    // Daily variation parameters
    double timeOfDay;    // 0-24 hours
    double dayNumber;    // Day counter

    // Random generators
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> tempNoise;
    std::normal_distribution<> humNoise;

    bool currentFireStatus;
    std::normal_distribution<> fireNoise;

    // Parameter hujan
    bool isRaining;
    double rainIntensity;  // 0-1 (0: tidak hujan, 1: hujan lebat)
    double rainProbability;
    std::uniform_real_distribution<> rainChance;
    std::uniform_real_distribution<> rainIntensityDist;

    // Parameter hujan yang mempengaruhi sensor
    double soilMoisture;
    double evaporationRate;

    double linearRisk(double z, double thresholdLow, double thresholdHigh) {
        if (z <= thresholdLow) return 0.0;
        if (z >= thresholdHigh) return 1.0;
        return (z - thresholdLow) / (thresholdHigh - thresholdLow); // Skala linear
    }

public:
    ForestEnvironment() :
        baseTemp(25.0),
        baseHumidity(70.0),
        timeOfDay(0),
        dayNumber(0),
        gen(rd()),
        tempNoise(0.0, 0.5),   // Small random variations
        humNoise(0.0, 1.0),     // Slightly larger variations for humidity
        currentFireStatus(false),
        fireNoise(0.0, 0.2),  // Noise for fire detection
        soilMoisture(50.0),    // Kelembaban tanah default 50%
        evaporationRate(0.1),    // Laju penguapan 10% per jam
        isRaining(false),
        rainIntensity(0.0),
        rainProbability(0.3), // 30% kemungkinan hujan setiap update
        rainChance(0.0, 1.0),
        rainIntensityDist(0.1, 1.0)  // Intensitas hujan minimal 0.1, maksimal 1.0
    {}

    // Update environment based on time
    void updateEnvironment(double time) {
        timeOfDay = fmod(time, 24.0);
        dayNumber = floor(time / 24.0);
        updateRainStatus();
    }

    // Get actual temperature (ground truth)
    double getRealTemperature() {
        double temp = baseTemp;

        // Efek pendinginan dari hujan dan evaporasi
        if (isRaining) {
            // Pendinginan lebih signifikan saat hujan
            temp -= (8.0 * rainIntensity);  // Turunkan suhu hingga 8°C saat hujan lebat
        } else if (soilMoisture > 50.0) {
            // Efek pendinginan dari evaporasi tanah basah
            temp -= (3.0 * (soilMoisture - 50.0) / 50.0);  // Maksimal turun 3°C dari evaporasi
        }

        // Variasi harian lebih teredam saat hujan
        double dailyVariationAmplitude = isRaining ?
            (3.0 * (1.0 - rainIntensity)) : // Amplitudo berkurang saat hujan
            5.0;  // Amplitudo normal

        double dailyVariation = dailyVariationAmplitude * sin(2 * M_PI * (timeOfDay - 6) / 24.0);
        double seasonalVariation = 3.0 * sin(2 * M_PI * dayNumber / 365.0);
        double weatherEffect = tempNoise(gen);

        return temp + dailyVariation + seasonalVariation + weatherEffect;
    }

    // Get actual humidity (ground truth)
    double getRealHumidity() {
        double baseHumidity = this->baseHumidity;

        // Pengaruh hujan dan kelembaban tanah
        if (isRaining) {
            baseHumidity += (20.0 * rainIntensity);
            soilMoisture = std::min(100.0, soilMoisture + (10.0 * rainIntensity));
        } else {
            // Penguapan dari tanah mempengaruhi kelembaban udara
            double evaporation = soilMoisture * evaporationRate;
            baseHumidity += evaporation;
            soilMoisture = std::max(0.0, soilMoisture - evaporation);
        }

        double temp = getRealTemperature();
        double baseHumidityVariation = -0.5 * (temp - this->baseTemp);
        double dailyVariation = -10.0 * sin(2 * M_PI * (timeOfDay - 6) / 24.0);
        double weatherEffect = humNoise(gen);

        // Tambahkan efek kelembaban tanah
        double soilEffect = 0.2 * soilMoisture;

        double humidity = baseHumidity + baseHumidityVariation + dailyVariation +
                         weatherEffect + soilEffect;

        return std::max(0.0, std::min(100.0, humidity));
    }

    double normalCDF(double z) {
        return 0.5 * erfc(-z / std::sqrt(2));  // erfc adalah komplementer dari fungsi kesalahan
    }

    double hasActiveFire(double temp, double humidity) {
        // Jika hujan sangat lebat, kemungkinan kebakaran sangat kecil
        if (isRaining && rainIntensity > 0.8) {
            return 0.0;
        }

        // Parameter distribusi normal untuk suhu dan kelembaban
        double meanTemp = 40.0;  // Rata-rata suhu untuk risiko kebakaran
        double stdDevTemp = 5.0; // Deviasi standar untuk suhu
        double meanHumid = 60.0; // Rata-rata kelembaban untuk risiko kebakaran
        double stdDevHumid = 5.0; // Deviasi standar untuk kelembaban

        // Sesuaikan threshold berdasarkan kondisi hujan
        if (isRaining) {
            meanTemp += (15.0 * rainIntensity);  // Butuh suhu lebih tinggi untuk kebakaran saat hujan
            meanHumid -= (10.0 * rainIntensity); // Sesuaikan threshold kelembaban saat hujan
        }

        // Menghitung nilai-z untuk suhu dan kelembaban
        double tempZScore = (temp - meanTemp) / stdDevTemp;
        double humidZScore = (humidity - meanHumid) / stdDevHumid;

        // Risiko berdasarkan transformasi linear
        double tempRisk = linearRisk(tempZScore, 0.0, 4.0);  // Risiko suhu dari z=0 hingga z=4
        double humidRisk = linearRisk(-humidZScore, 0.0, 3.0); // Risiko kelembaban rendah dari z=-3 hingga z=0

        // Kombinasikan risiko menggunakan maksimum
        double fireRisk = std::max(tempRisk, humidRisk);

        // Kurangi risiko berdasarkan kondisi hujan
        if (isRaining) {
            fireRisk *= (1.0 - (0.8 * rainIntensity)); // Hujan mengurangi risiko hingga 80%
        }

        // Kurangi risiko berdasarkan kelembaban tanah
        if (soilMoisture > 70.0) {
            fireRisk *= (1.0 - ((soilMoisture - 70.0) / 100.0));
        }

        // Menjaga probabilitas agar tetap antara 0 dan 1
        return std::max(0.0, std::min(1.0, fireRisk));
    }

    // Getter tambahan
    double getSoilMoisture() const { return soilMoisture; }

    // Tambahkan method baru
    void updateRainStatus() {
        // Update status hujan setiap 3 jam
        if (fmod(timeOfDay, 3.0) < 0.1) {  // Check setiap ~3 jam
            if (rainChance(gen) < rainProbability) {
                isRaining = true;
                rainIntensity = rainIntensityDist(gen);
            } else {
                isRaining = false;
                rainIntensity = 0.0;
            }
        }
    }

    bool getIsRaining() const { return isRaining; }
    double getRainIntensity() const { return rainIntensity; }
};

#endif
