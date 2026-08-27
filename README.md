<div align="center">

# 🌲 Jaringan Sensor di Hutan dengan LoRa
### Simulasi Kinerja Jaringan Nirkabel Menggunakan OMNeT++ dengan Berbagai Teknologi Komunikasi

_"Memantau hutan tanpa kabel, menjaga ekosistem dari jarak jauh."_

[![OMNeT++](https://img.shields.io/badge/OMNeT%2B%2B-Simulation-red?style=for-the-badge)]()
[![FLoRa](https://img.shields.io/badge/Framework-FLoRa-orange?style=for-the-badge)]()
[![INET](https://img.shields.io/badge/INET-4.4-blue?style=for-the-badge)]()
[![FILKOM UB](https://img.shields.io/badge/FILKOM-Universitas%20Brawijaya-003366?style=flat-square)]()
[![Status](https://img.shields.io/badge/Status-Selesai-brightgreen?style=flat-square)]()

<img src="https://img.shields.io/badge/📡%20Node%20Sensor-10-blue?style=flat-square" />
<img src="https://img.shields.io/badge/📶%20Frekuensi-915%20MHz-blue?style=flat-square" />
<img src="https://img.shields.io/badge/🗺️%20Area%20Cakupan-500x500m-blue?style=flat-square" />

</div>

---

## 📌 Daftar Isi
- [Tentang Project](#-tentang-project)
- [Rumusan Masalah & Tujuan](#-rumusan-masalah--tujuan)
- [Topologi Jaringan](#-topologi-jaringan)
- [Parameter Sistem](#️-parameter-sistem)
- [Tech Stack](#️-tech-stack)
- [Struktur Direktori](#-struktur-direktori)
- [Cara Menjalankan Simulasi](#-cara-menjalankan-simulasi)
- [Ringkasan Hasil](#-ringkasan-hasil)
- [Rekomendasi](#-rekomendasi)
- [Tim & Pembagian Tugas](#-tim--pembagian-tugas)

---

## 📖 Tentang Project

Project ini mensimulasikan **jaringan sensor nirkabel berbasis LoRa (Long Range)** untuk pemantauan kondisi hutan, menggunakan **OMNeT++** dengan framework **FLoRa** dan **INET 4.4**. Sistem dirancang untuk memantau tiga parameter lingkungan utama secara real-time dari lokasi terpencil dengan konsumsi daya rendah:

- 🌡️ **Suhu lingkungan** — deteksi perubahan iklim mikro & potensi kebakaran
- 💧 **Kelembaban tanah** — indikator kesuburan & risiko kekeringan
- 🔥 **Potensi kebakaran** — deteksi dini gangguan ekosistem

Latar belakangnya: pengelolaan hutan di Indonesia menghadapi tantangan degradasi lingkungan dan kebakaran yang sering kali tidak terpantau secara real-time. Teknologi **LoRa** dipilih karena mampu mengirim data jarak jauh (hingga 15 km di area rural) dengan konsumsi daya sangat rendah dan penetrasi sinyal yang baik melalui vegetasi — cocok untuk node sensor yang harus bertahan lama tanpa perawatan intensif di tengah hutan.

Project ini dikerjakan sebagai tugas **Mata Kuliah Jaringan Nirkabel TIF - Kelas F**, di bawah bimbingan **Eko Sakti Pramukantoro, S.Kom., M.Kom., Ph.D.**

---

## 🎯 Rumusan Masalah & Tujuan

1. Bagaimana menentukan penempatan optimal sensor LoRa untuk cakupan efektif dengan jumlah node minimal?
2. Seberapa efektif jaringan sensor LoRa dalam mendeteksi dan mengirimkan data lingkungan di area hutan?
3. Bagaimana pengaruh kondisi lingkungan terhadap performa jaringan LoRa dalam hal komunikasi dan efisiensi energi?
4. Bagaimana jaringan sensor ini dapat mendukung pemantauan ekosistem hutan secara menyeluruh?

Tujuannya: merancang, menguji, dan mengoptimalkan konfigurasi jaringan LoRa untuk keseimbangan antara **jangkauan komunikasi**, **efisiensi energi**, dan **keandalan data**.

---

## 🗺️ Topologi Jaringan

```text
                          ┌───────────┐
                          │  Gateway  │
                          │ (250,250) │
                          │ Coverage: │
                          │  50m efektif │
                          └─────┬─────┘
        ┌──────┬──────┬──────┬─┴┬──────┬──────┬──────┬──────┐
      10-50m 15-45m 20-40m 25-35m 30-45m 35-50m 25-40m 20-45m 15-35m 10-30m
        │      │      │      │      │      │      │      │      │      │
      Node0  Node1  Node2  Node3  Node4  Node5  Node6  Node7  Node8  Node9
```

- **Gateway LoRaWAN** — pengumpul & forwarding data, backhaul via cellular/satellite
- **10 Node Sensor** — tersebar di area hutan 500×500 meter, jarak ke gateway 10–50 meter, mode *sleep* saat idle untuk hemat daya
- Alur data: **Node Sensor → Gateway → Network Server (via UDP)**

---

## ⚙️ Parameter Sistem

<details>
<summary><b>Klik untuk lihat detail parameter lengkap</b></summary>

**Parameter Kanal**
| Parameter | Nilai | Keterangan |
|---|---|---|
| Frekuensi | 915 MHz | Standar Indonesia |
| Bandwidth | 125 kHz | Standar LoRa |
| Coding Rate | 4/6 | Redundansi lebih tinggi untuk keandalan |
| Spreading Factor | 8–11 | Optimasi jarak & konsumsi daya |
| Transmission Power | 13–17 dBm | Disesuaikan regulasi lokal |

**Jarak Antar Node**
| Parameter | Nilai |
|---|---|
| Jarak minimal antar sensor | 12 meter |
| Jarak maksimal antar sensor | 48 meter |
| Jarak minimal ke gateway | 10 meter |
| Jarak maksimal ke gateway | 50 meter |

**Sumber Daya**
| Mode | Konsumsi |
|---|---|
| Idle | 0.0003 mA |
| Receiving | 10.5 mA |
| Transmit (TP 13–17) | 29–45 mA |

**Protokol & Pengaturan Sensor**
| Parameter | Nilai |
|---|---|
| Kelas LoRaWAN | Class A (optimasi baterai) |
| Framework | FLoRa |
| Enkripsi | AES-256 |
| Data Rate | 300–6.5k bps (adaptive) |
| Duty Cycle | 0.8% |
| Interval Sampling | 4 menit |
| Interval Transmisi | 12 menit |
| Payload Size | 24 bytes |
| Retry Attempt | 4x, timeout 2.5 detik |

</details>

---

## 🛠️ Tech Stack

<div align="center">

| Kategori | Teknologi |
|---|---|
| **Simulator** | OMNeT++ |
| **Framework** | FLoRa · INET 4.4 |
| **Protokol** | LoRa / LoRaWAN Class A |
| **Model Propagasi** | Log-Normal Shadowing Path Loss |
| **Visualisasi Peta** | OpenStreetMap |
| **Analisis Data** | Python (Matplotlib), OMNeT++ IDE Scalars/Vectors |

</div>

---

## 📂 Struktur Direktori

```text
JaringanNirkabelProject-SensorHutan/
├── flora/                                                    # Project simulasi OMNeT++ (FLoRa + INET 4.4)
│   └── simulations/
│       ├── jarsensorhutansansen.ini                          # Konfigurasi node, gateway, & parameter kanal
│       ├── jarsensorhutansansen.ned                           # Layout network (coordinateSystem, visualizer, dll)
│       └── map.osm                                            # Peta area simulasi (OpenStreetMap)
├── Laporan Project Jaringan Nirkabel_TIF FILKOM UB.pdf        # Laporan lengkap project
└── README.md
```

---

## ▶️ Cara Menjalankan Simulasi

> **Prasyarat:** OMNeT++ dengan simulation model **INET 4.4** dan **FLoRa** sudah ter-install.

1. Pilih & export peta area simulasi dari [openstreetmap.org](https://openstreetmap.org) → simpan sebagai `map.osm` di folder `simulations/`
2. Buka `map.osm` di OMNeT++, salin nilai `maxlat` & `minlon`, lalu tempel ke `sceneLatitude`/`sceneLongitude` pada `jarsensorhutansansen.ini`
3. Sesuaikan `numberOfPacketsToSend`, `sim-time-limit`, `repeat`, jumlah node/gateway, serta `initialLoRaCR`/`SF`/`TP` sesuai use case
4. Klik kanan `jarsensorhutansansen.ini` → **Run As** → **OMNeT++ Simulation**, lalu jalankan hingga tampilan StreetMap muncul
5. Klik tombol **Run** (▶️▶️▶️ hijau) hingga simulasi selesai
6. Hasil output tersimpan sebagai `hutsansansen-s0.ini.sca` di folder `results/` — buka lewat tab **Browse Data → Vectors/Scalars** untuk melihat SNIR, RSSI, konsumsi energi, dan data retention

---

## 📊 Ringkasan Hasil

| Metrik | Jarak Dekat (10-20m) | Jarak Menengah (30-40m) | Jarak Jauh (50m) |
|---|---|---|---|
| **RSSI** | ~-113 s.d -120 dBm | ~-115 s.d -120 dBm | ~-102 dBm |
| **SNIR** | Relatif baik (-10 s.d -14 dB) | Menurun (-14 s.d -77 dB) | Degradasi signifikan (hingga -1.056 dB) |
| **Data Retention** | 100% | 95–98% | 56–58% |
| **Konsumsi TX** | ~16.6–17.9 kmW | ~13.4–18.9 mW | ~15.3–16 mW |
| **Konsumsi Idle** | ~0.0001287 mW | — | ~0.00013 mW (stabil di semua jarak) |

**Deteksi sensor:**
- 🔥 Deteksi kebakaran mampu membedakan kejadian nyata dari *false alarm* dengan noise terkendali
- 🌡️ Error suhu terjaga dalam rentang **±0.4°C**
- 💧 Error kelembaban stabil di rentang **-2 hingga 5 satuan**, dengan recovery cepat setelah gangguan

> **Insight utama:** jaringan mempertahankan reliabilitas >95% hingga jarak 30 meter, namun mengalami penurunan tajam (retention 56-58%) pada jarak 50 meter — menunjukkan perlunya penempatan node lebih rapat atau repeater untuk cakupan hutan yang luas.

---

## 💡 Rekomendasi

1. **Penempatan node** — batasi jarak antar node maksimal 30 meter untuk reliabilitas >95%; gunakan topologi mesh + repeater untuk area SNIR rendah
2. **Parameter transmisi** — terapkan TP & SF adaptif berdasarkan jarak dan kondisi kanal
3. **Manajemen energi** — optimasi duty cycle, algoritma sleep/wake, dan energy harvesting untuk node berkonsumsi tinggi
4. **Keandalan sistem** — redundansi data dan validasi silang antar sensor untuk tingkatkan akurasi deteksi

---

## 👥 Tim & Pembagian Tugas

| Fase | PIC | Deskripsi |
|---|---|---|
| Penelitian Pendahuluan | Deo, Yoel | Studi literatur LoRa, OMNeT++, jaringan sensor |
| Perancangan Parameter & Topologi | Hasan, Husain, Yoel | Parameter sistem, sketsa topologi |
| Implementasi Simulasi | Husain, Hasan | Konfigurasi node, gateway, kanal komunikasi di OMNeT++ |
| Pengujian & Pengumpulan Data | Husain, Hasan | Uji jangkauan, konsumsi daya, akurasi, retention rate |
| Analisis & Visualisasi Hasil | Hasan, Husain | Analisis SNIR, RSSI, & performa lainnya |
| Penyusunan Laporan Akhir | Semua anggota | Laporan, kesimpulan, & presentasi final |

**Anggota:**
1. Muhammad Hasan Fadhlillah — 225150207111026
2. Muhammad Husain Fadhlillah — 225150207111027
3. Eleazar Tadeo Eman — 225150201111053
4. Yoel Amadeo P — 225150207111009

**Dosen Pengampu:** Eko Sakti Pramukantoro, S.Kom., M.Kom., Ph.D.
*Program Studi Teknik Informatika, Fakultas Ilmu Komputer, Universitas Brawijaya — 2024*

---

<div align="center">

**Jaringan Sensor di Hutan dengan LoRa**
Solusi pemantauan hutan hemat daya untuk mendukung konservasi ekosistem 🌲🇮🇩

</div>
