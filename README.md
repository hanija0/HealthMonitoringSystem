

# 🏥 Health Monitoring System

A **cross-platform Health Monitoring System** written in **C**, designed to run on **Windows, Linux, and macOS**.

---

## ✨ Features

* 🔐 User registration & secure login (hashed passwords)
* ➕ Add / View / Edit / Delete BMI records
* ⚠️ BMI risk analysis based on age, weight, and height
* 📊 Statistics report

  * Average BMI
  * Minimum BMI
  * Maximum BMI
* 📈 ASCII BMI trend chart
* 📁 Export reports to CSV
* 💻 Cross-platform compatibility (Windows, Linux, macOS)
* 🛡️ Safe input handling and robust file operations

---

## ⚙️ Compile & Run

### 🍎 macOS / 🐧 Linux

```bash
gcc health_system.c -o health
./health
```

### 🪟 Windows (MinGW / GCC)

```bash
gcc health_system.c -o health.exe
health.exe
```

---

## 📂 Project Structure

```
.
├── health_system.c
├── users.txt (when run)
├── health_records.csv (when run)
├── reports/ (will be saved)
├── .gitignore
└── README.md
```

---

## 📊 BMI Formula

```
BMI = weight (kg) / (height (m) × height (m))
```

### Risk Levels

| BMI Range   | Risk     |
| ----------- | -------- |
| < 18.5      | Moderate |
| 18.5 – 24.9 | Low      |
| 25 – 29.9   | High     |
| ≥ 30        | Critical |

---

## 📁 CSV Export

* Reports are saved in the `reports/` directory
* Timestamp-based filenames ensure no overwrites

---

## 🚀 Future Enhancements

* Password masking
* Encrypted data storage
* Graphical user interface
* PDF export
* Unit testing

---

## 📜 License

This project is open-source and intended for **educational use**.
