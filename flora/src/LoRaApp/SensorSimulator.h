#ifndef SENSOR_SIMULATOR_H
#define SENSOR_SIMULATOR_H

#include <random>
#include <cmath>

class SensorSimulator {
private:
    // Karakteristik sensor dasar
    double tempAccuracy;
    double humidityAccuracy;

    // Karakteristik sensor saat hujan
    double wetTempAccuracy;    // Akurasi suhu saat basah
    double wetHumidityAccuracy; // Akurasi kelembaban saat basah

    // Parameter kondensasi
    double condensationThreshold; // Ambang batas kelembaban untuk kondensasi
    bool condensatingStatus;

    // Random generators
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> tempError;
    std::normal_distribution<> humError;
    std::normal_distribution<> wetTempError;
    std::normal_distribution<> wetHumErr;

    // Drift parameters
    double tempDrift;
    double humDrift;
    double driftRate;
    double wetDriftRate;  // Drift lebih cepat saat basah

    double linearRisk(double z, double thresholdLow, double thresholdHigh) {
        if (z <= thresholdLow) return 0.0;
        if (z >= thresholdHigh) return 1.0;
        return (z - thresholdLow) / (thresholdHigh - thresholdLow); // Skala linear
    }

public:
    SensorSimulator() :
        tempAccuracy(0.5),      // ±0.5°C normal accuracy
        humidityAccuracy(2.0),  // ±2% normal accuracy
        wetTempAccuracy(1.0),   // ±1.0°C wet accuracy
        wetHumidityAccuracy(4.0), // ±4% wet accuracy
        condensationThreshold(85.0), // Kondensasi pada RH > 85%
        condensatingStatus(false),
        gen(rd()),
        tempError(0.0, tempAccuracy/2),
        humError(0.0, humidityAccuracy/2),
        wetTempError(0.0, wetTempAccuracy/2),
        wetHumErr(0.0, wetHumidityAccuracy/2),
        tempDrift(0),
        humDrift(0),
        driftRate(0.0001),
        wetDriftRate(0.0005)    // 5x lebih cepat drift saat basah
    {}

    // Simulasi pembacaan suhu dengan pengaruh hujan
    double readTemperature(double realTemp, bool isRaining = false, double rainIntensity = 0.0) {
        double noise;
        double currentDriftRate = driftRate;

        if (isRaining) {
            // Gunakan error yang lebih besar saat hujan
            noise = wetTempError(gen) * (1 + rainIntensity);
            currentDriftRate = wetDriftRate * rainIntensity;

            // Tambahkan efek pendinginan tambahan karena sensor basah
            realTemp -= (2.0 * rainIntensity); // Sensor bisa membaca lebih dingin saat basah
        } else {
            noise = tempError(gen);
        }

        // Update drift
        tempDrift += (currentDriftRate * tempError(gen));

        // Tambahkan delay respon sensor dan efek evaporative cooling
        double sensorLag = isRaining ? (1.0 * rainIntensity) : 0.0;
        double evaporativeCooling = isRaining ? (3.0 * rainIntensity) : 0.0;

        return realTemp + noise + tempDrift + sensorLag - evaporativeCooling;
    }

    // Simulasi pembacaan kelembaban dengan pengaruh hujan
    double readHumidity(double realHumidity, bool isRaining = false, double rainIntensity = 0.0) {
        double noise;
        double currentDriftRate = driftRate;

        // Cek kondensasi
        condensatingStatus = (realHumidity > condensationThreshold) || isRaining;

        if (isRaining || condensatingStatus) {
            // Error lebih besar saat hujan atau kondensasi
            noise = wetHumErr(gen) * (1 + rainIntensity);
            currentDriftRate = wetDriftRate * (isRaining ? rainIntensity : 0.5);
        } else {
            noise = humError(gen);
        }

        // Update drift
        humDrift += (currentDriftRate * humError(gen));

        // Tambahkan saturasi saat hujan lebat
        double saturationEffect = 0.0;
        if (isRaining && rainIntensity > 0.7) {
            saturationEffect = 5.0 * (rainIntensity - 0.7);
        }

        double reading = realHumidity + noise + humDrift + saturationEffect;

        // Clamp ke range valid
        return std::max(0.0, std::min(100.0, reading));
    }

    double normalCDF(double z) {
        return 0.5 * std::erfc(-z / std::sqrt(2));  // erfc adalah komplementer dari fungsi kesalahan
    }

    // Deteksi kebakaran dengan mempertimbangkan hujan
    double detectPotentialFire(double temp, double humidity, bool isRaining = false, double rainIntensity = 0.0) {
        // Jika hujan sangat lebat, sensor mendeteksi kemungkinan kebakaran sangat kecil
        if (isRaining && rainIntensity > 0.8) {
            return 0.0;
        }

        // Parameter distribusi normal untuk suhu dan kelembaban
        double meanTemp = 40.0;  // Rata-rata suhu untuk risiko kebakaran
        double stdDevTemp = 5.0; // Deviasi standar untuk suhu
        double meanHumid = 58.0; // Rata-rata kelembaban untuk risiko kebakaran
        double stdDevHumid = 5.0; // Deviasi standar untuk kelembaban

        // Sesuaikan threshold berdasarkan kondisi hujan
        if (isRaining) {
            meanTemp += (15.0 * rainIntensity);  // Butuh suhu lebih tinggi untuk deteksi saat hujan
            meanHumid -= (10.0 * rainIntensity); // Sesuaikan threshold kelembaban saat hujan
        }

        // Menghitung nilai-z untuk suhu dan kelembaban
        double tempZScore = (temp - meanTemp) / stdDevTemp;
        double humidZScore = (humidity - meanHumid) / stdDevHumid;

        // Tambahkan noise pada pembacaan sensor saat hujan
        if (isRaining) {
            tempZScore += wetTempError(gen) * rainIntensity;
            humidZScore += wetHumErr(gen) * rainIntensity;
        }

        // Risiko berdasarkan transformasi linear
        double tempRisk = linearRisk(tempZScore, 0.0, 4.0);  // Risiko suhu dari z=0 hingga z=4
        double humidRisk = linearRisk(-humidZScore, 0.0, 3.0); // Risiko kelembaban rendah dari z=-3 hingga z=0

        // Kombinasikan risiko menggunakan maksimum
        double fireRisk = std::max(tempRisk, humidRisk);

        // Kurangi risiko berdasarkan kondisi hujan
        if (isRaining) {
            fireRisk *= (1.0 - (0.8 * rainIntensity)); // Hujan mengurangi risiko hingga 80%
        }

        // Menjaga probabilitas agar tetap antara 0 dan 1
        return std::max(0.0, std::min(1.0, fireRisk));
    }

    // Getter untuk status sensor
    bool isCondensating() const { return condensatingStatus; }
    double getTemperatureDrift() const { return tempDrift; }
    double getHumidityDrift() const { return humDrift; }
};

#endif
