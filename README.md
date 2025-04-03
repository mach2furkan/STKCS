

# STKCS - Secure Threaded Chat System  
![C++](https://img.shields.io/badge/C++-17-blue?logo=cplusplus) ![License](https://img.shields.io/badge/License-MIT-green) ![Educational](https://img.shields.io/badge/For-Education%20Only-red) ![Warning](https://img.shields.io/badge/Warning-Not%20For%20Production-critical)

## 🌐 Languages
[English](#english) | [Deutsch](#deutsch) | [Türkçe](#türkçe)

---


## 🇬🇧 ENGLISH

### ⚠️ STRICT LEGAL NOTICE
**This project is EXCLUSIVELY FOR ACADEMIC RESEARCH AND COMPUTER SCIENCE EDUCATION.** By using this software, you agree to the following binding terms:

1. **Commercial Prohibition**  
   Any commercial application, monetization, or business-related use is **expressly forbidden** under copyright law.

2. **Security Disclaimer**  
   This implementation contains **deliberate security weaknesses** for pedagogical demonstration. Never deploy in production environments.

3. **Liability Waiver**  
   The author bears **no responsibility** for:  
   - Any damages caused by misuse  
   - Security breaches resulting from implementation  
   - Legal violations by end-users  

4. **Patent Warning**  
   Reverse-engineering or reusing code may violate software patents in your jurisdiction.

```legal
Copyright Holder: Furkan Aşkın
Allowed Use Cases: Classroom teaching, academic papers, security research
Forbidden Use Cases: Real communication systems, commercial products, government systems
```

### 🔬 Academic Features
| Component         | Educational Purpose                |
|-------------------|------------------------------------|
| XOR Encryption    | Cryptography fundamentals          |
| Thread Pool       | Concurrency patterns               |
| Socket API        | Network programming basics         |
| Message Queues    | IPC mechanisms demonstration       |

### 📦 Compilation Guide
```bash
# Requires CMake 3.15+ and C++17 compiler
git clone https://github.com/mach2furkan/STKCS.git
cd STKCS
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4
```

---

<a id="deutsch"></a>
## 🇩🇪 DEUTSCH

### ⚠️ RECHTSVERBINDLICHE HINWEISE
**Dieses Projekt dient AUSSCHLIESSLICH ACADEMISCHEN ZWECKEN.** Durch die Nutzung erkennen Sie folgende Bedingungen an:

1. **Kommerzielle Nutzungsverbot**  
   Jeglicher gewerblicher Einsatz ist **strengstens untersagt**.

2. **Sicherheitshinweis**  
   Diese Implementierung enthält **absichtliche Schwachstellen** zu Lehrzwecken.

3. **Haftungsausschluss**  
   Der Autor übernimmt **keine Haftung** für:  
   - Schäden durch Missbrauch  
   - Sicherheitslücken  
   - Rechtsverstöße Dritter  

```legal
Urheber: Furkan Aşkın
Erlaubter Nutzungsbereich: Lehre, Forschung  
Verboten: Produktiveinsatz, kommerzielle Verwertung
```

### 🧪 Akademische Merkmale
| Komponente        | Lehrzweck                          |
|-------------------|------------------------------------|
| XOR-Verschlüsselung | Kryptographie-Grundlagen          |
| Thread-Management | Nebenläufige Programmierung        |

---

<a id="türkçe"></a>
## 🇹🇷 TÜRKÇE

### ⚠️ KESİN YASAL UYARI
**Bu proje SADECE BİLİMSEL ARAŞTIRMA VE EĞİTİM AMAÇLIDIR.** Kullanımınızla birlikte aşağıdaki şartları kabul etmiş sayılırsınız:

1. **Ticari Yasak**  
   Tüm ticari kullanımlar **açıkça yasaktır**.

2. **Güvenlik Uyarısı**  
   Bu yazılım **bilerek zayıf şifreleme** içerir.

3. **Sorumluluk Reddi**  
   Yazar şunlardan **sorumlu değildir**:  
   - Kötüye kullanım sonucu oluşan zararlar  
   - Güvenlik ihlalleri  

```legal
Telif Sahibi: Furkan Aşkın
İzin Verilen Kullanım: Üniversite eğitimi, bilimsel makaleler
Yasak Kullanım: Gerçek iletişim sistemleri, ticari ürünler
```

### 🔍 Detaylı Teknik Özellikler
```cpp
// ÖRNEK KOD: Sadece eğitim amaçlı zayıf şifreleme
string sifrele(const string& metin) {
    string sifreli = metin;
    for(char& c : sifreli) {
        c ^= 0xAA; // XOR işlemi (güvensiz)
    }
    return sifreli; 
}
```

---

## 📜 Global License Terms
**By using this software, you automatically agree to:**  
1. Educational Use Only Clause  
2. No-Warranty Agreement  
3. Jurisdiction in Istanbul/Türkiye  

![Legal Warning](https://img.shields.io/badge/LEGAL_WARNING-Any_commercial_use_will_be_prosecuted-red)

---

### Key Enhancements:
1. **Stronger Legal Language** with jurisdiction specifics
2. **Multi-level Warnings** in each language section
3. **Technical Compliance Tables** showing educational intent
4. **Code Samples** marked as insecure
5. **Automated Legal Badges** in headers
6. **Binding Terms Formatting** using legal code blocks

Would you like me to:
1. Add a DMCA takedown notice template?
2. Include export control warnings (EAR/ITAR)?
3. Add a vulnerability disclosure policy?
