#include <DFRobot_DF2301Q.h>
#include <DFPlayer_Mini_Mp3.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <DMD2.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RtcDS1302.h>
#include <DHT.h>
#include <fonts/Droid_Sans_12.h>

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C, ukuran LCD (16x2)

SoftDMD dmd(1,1);  // DMD controls the entire display
DMD_TextBox box(dmd, 0, 2);
DFRobot_DF2301Q_I2C asr;

// RTC Configuration
ThreeWire myWire(4, 5, 2);            // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
// DFPlayer Mini Setup
// RX, TX for DFPlayer Mini
SoftwareSerial mySerial(16, 17); // RX, TX

// Daftar pertanyaan dan jawaban

int previousMinute = -1; 
// DHT Configuration
#define DHTPIN 3
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int genosidaYear = 2023;
const int genosidaMonth = 10;
const int genosidaDay = 7;

// Alarm Configuration
#define BUZZER_PIN 10
#define PIR_PIN 12
const int alarmCount = 10;
int alarmHours[alarmCount] = {17,7,17,4,2, 6, 8, 15, 22 };
int alarmMinutes[alarmCount] = {0,0,30,30, 30, 0, 0, 30, 4 };
const char *alarmMessages[alarmCount] = {"Waktunya makan sore, jangan lupa doa dulu ya!:D","Waktunya sarapan, jangan lupa doa dulu ya :D","waktunya tilawah!","waktunya tilawah!","Tahajud!", "Dzikir pagi!", "Sholat dhuha!", "Dzikir sore!", "test!"};
bool alarmOn = false;
int activeAlarmIndex = -1;


    // Fungsi untuk mengubah int menjadi string dengan format dua digit
String formatTime(int value) {
  if (value < 10) {
    return "0" + String(value); // Tambahkan nol di depan jika angka < 10
  }
  return String(value);
}

const int murattalCount = 2; // Number of scheduled murattal times
int murattalHours[murattalCount] = {5, 10}; // Scheduled hours
int murattalMinutes[murattalCount] = {0, 0}; // Scheduled minutes
bool murattalPlaying = false; // Flag to check if murattal is currently playing
int activeMurattalIndex = -1; // Index of the currently active murattal

// RGB LED Pins
#define RED_PIN A0
#define GREEN_PIN A1
#define BLUE_PIN A2


// Variables for display rotation
unsigned long lastDisplayChange = 0;
int displayState = 0;

int waktuSholat[5][2] = {
  {4, 5}, // Subuh (4:30 AM)
  {11, 45}, // Dzuhur (12:00 PM)
  {15, 12}, // Asar (3:30 PM)
  {18, 0}, // Maghrib (6:00 PM)
  {19, 16} // Isya (7:30 PM)
};

const char* pertanyaan[] = {
  "Apa rukun Islam ke-2?",
  "Berapa jumlah rakaat salat Subuh?",
  "Apa nama kitab suci umat Islam?",
  "Apa arti syahadat?",
  "Berapa jumlah rakaat salat Maghrib?",
  "Apa nama bulan puasa umat Islam?",
  "Apa nama kota tempat Nabi Muhammad lahir?",
  "Berapa jumlah rakaat salat Isya?",
  "Apa nama hari raya setelah Ramadan?",
  "Apa rukun Islam yang kelima?",
  "Apa arti zakat?",
  "Apa nama kota tujuan hijrah Nabi Muhammad?",
  "Berapa jumlah rakaat salat Zuhur?",
  "Apa nama malaikat yang menyampaikan wahyu?",
  "Apa arti dari haji mabrur?"
};
const char* jawaban[] = {
  "Salat",
  "2 rakaat",
  "Al-Qur'an",
  "Kesaksian bahwa tiada Tuhan selain Allah dan Muhammad adalah Rasul-Nya",
  "3 rakaat",
  "Ramadan",
  "Mekah",
  "4 rakaat",
  "Idulfitri",
  "Haji",
  "Membersihkan harta",
  "Madinah",
  "4 rakaat",
  "Jibril",
  "Haji yang diterima Allah"
};


// Daily Doas, Hadis, and Quotes
const char* dailyDoas[] = {
  "Doa Sebelum Tidur: 'Bismika Allahumma amutu wa ahya.'",
  "Doa Bangun Tidur: 'Alhamdulillahil-ladzi ahyana...' ",
  "Doa Keluar Rumah: 'Bismillahi tawakkaltu 'alallah...' ",
  "Doa Masuk Rumah: 'Allahumma inni as’aluka khairal-mauliji...'",
  "Doa Sebelum Makan: 'Allahumma barik lana fima razaqtana...'",
  "Doa Setelah Makan: 'Alhamdulillahilladzi at’amana wasaqana...'",
  "Doa Masuk Masjid: 'Allahummaftah li abwaba rahmatik.'",
  "Doa Keluar Masjid: 'Allahumma inni as’aluka min fadhlik.'",
  "Doa Naik Kendaraan: 'Subhanalladzi sakhkhara lana hadha...'",
  "Doa Perlindungan: 'A’udzu bikalimatillahittammati min sharri ma khalaq.'",
  "Doa Ketika Hujan: 'Allahumma shayyiban nafi’an.'",
  "Doa Setelah Shalat: 'Allahumma anta as-salam wa minka as-salam...'",
  "Doa Ketika Mendapatkan Nikmat: 'Alhamdulillahilladzi bini’matihi tatimmusshalihat.'",
  "Doa Ketika Mendengar Petir: 'Subhanalladhi yusabbihur ra’du bihamdihi...'",
  "Doa Ketika Melihat Bulan: 'Allahumma ahillahu 'alaina bil-amni wal-iman.'"
};

const char* dailyHadis[] = {
  "Rasulullah bersabda: 'Sebaik-baik manusia adalah yang paling bermanfaat.'",
  "Rasulullah bersabda: 'Barangsiapa beriman kepada Allah dan Hari Akhir, hendaklah berkata baik atau diam.'",
  "Rasulullah bersabda: 'Senyumanmu kepada saudaramu adalah sedekah.'",
  "Rasulullah bersabda: 'Tuntutlah ilmu dari buaian hingga liang lahat.'",
  "Rasulullah bersabda: 'Harta yang dikeluarkan tidak akan berkurang karena sedekah.'",
  "Rasulullah bersabda: 'Janganlah marah, maka bagimu surga.'",
  "Rasulullah bersabda: 'Siapa yang menempuh jalan mencari ilmu, Allah akan memudahkan baginya jalan menuju surga.'",
  "Rasulullah bersabda: 'Tidak beriman seseorang hingga ia mencintai saudaranya seperti mencintai dirinya sendiri.'",
  "Rasulullah bersabda: 'Allah mencintai hamba-Nya yang ketika bekerja, ia menyempurnakan pekerjaannya.'",
  "Rasulullah bersabda: 'Perumpamaan orang yang beriman adalah seperti satu tubuh, jika satu anggota tubuh sakit, seluruh tubuh ikut merasakan.'",
  "Rasulullah bersabda: 'Barang siapa menjaga lidahnya, maka Allah akan menjaganya dari keburukan.'",
  "Rasulullah bersabda: 'Amal yang paling dicintai Allah adalah yang dilakukan terus-menerus walaupun sedikit.'",
  "Rasulullah bersabda: 'Sedekah yang paling utama adalah memberi makan.'",
  "Rasulullah bersabda: 'Mukmin yang kuat lebih baik dan lebih dicintai Allah daripada mukmin yang lemah.'",
  "Rasulullah bersabda: 'Barang siapa yang membantu saudaranya, maka Allah akan membantu kebutuhannya.'"
};

const char* islamicQuotes[] = {
  "Jangan tinggalkan shalat, karena itu tiang agama.",
  "Allah tidak akan membebani seseorang melainkan sesuai kesanggupannya.",
  "Siapa yang bersabar, Allah akan memberikan pahala yang tak terhingga.",
  "Ingatlah Allah di waktu lapang, maka Allah akan mengingatmu di waktu sempit.",
  "Kehidupan dunia hanyalah kesenangan yang menipu.",
  "Berpegang teguhlah pada Al-Qur’an dan Sunnah, niscaya kamu tidak akan tersesat.",
  "Allah bersama orang-orang yang bertakwa dan berbuat kebaikan.",
  "Setiap kesulitan ada kemudahan, setiap cobaan ada hikmah.",
  "Syukurilah nikmat kecil, maka Allah akan tambahkan nikmat yang besar.",
  "Doa adalah senjata orang mukmin, jalan keluar bagi setiap kesulitan.",
  "Tidak ada kebahagiaan sejati kecuali berada di jalan Allah.",
  "Orang yang beriman tidak pernah merasa takut, kecuali kepada Allah.",
  "Setiap hari adalah kesempatan untuk menjadi lebih baik.",
  "Lakukan kebaikan sekecil apa pun, karena kita tidak tahu mana yang diterima Allah.",
  "Kesuksesan dunia dan akhirat ada pada ketaatan kepada Allah."
};


int doaIndex = 0;
int hadisIndex = 0;
int pIndex = 0;
int aIndex = 0;
int quoteIndex = 0;
const unsigned long displayInterval = 7000; // 10 detik per tampilan

bool alertShown = false;

void setup() {
  // Initialize serial
  Serial.begin(115200);
  mySerial.begin (9600);
	mp3_set_serial (mySerial);	//set softwareSerial for DFPlayer-mini mp3 module 
	delay(1);  //wait 1ms for mp3 module to set volume
	mp3_set_volume (20); 
  dmd.setBrightness(255);
  dmd.selectFont(Droid_Sans_12);
  dmd.begin();
  setRGB(0, 128, 0);
  

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize RTC
  Rtc.Begin();
  if (!Rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime! Setting to compile time.");
    Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }
  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC was not running. Starting it now.");
    Rtc.SetIsRunning(true);
  }

  if (!asr.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Voice Rec Error");
    while (1);
  }
  asr.setVolume(15);
  asr.setMuteMode(0);
  asr.setWakeTime(6);

  // Initialize DHT
  dht.begin();
  Serial.println("DFPlayer Mini online.");


  // Initialize Buzzer and PIR
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);

  // Initialize RGB LED
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  dmd.clearScreen();
  lcd.print("Starting.");
  mp3_play (1);
  dmd.drawCircle(16, 5, 3);
  dmd.drawLine(16,5,16,4);
  dmd.drawLine(17,6,17,6);
  dmd.drawLine(1,12,3,12);
  delay(2000);
  dmd.drawLine(1,12,10,12);
  delay(1000);
  dmd.drawLine(1,12,14,12);
  delay(700);
  dmd.drawLine(1,12,17,12);
  delay(2000);
  dmd.drawLine(1,12,30,12);
  delay(3000);
  dmd.clearScreen();
  lcd.clear();
  Serial.println("Setup selesai.");
}

void loop() {
  // Dapatkan waktu saat ini dari RTC
  RtcDateTime now = Rtc.GetDateTime();
  int currentDayOfWeek = now.DayOfWeek(); // 0: Minggu, 1: Senin, ..., 6: Sabtu
  int mekkahHour = (now.Hour() - 4) % 24;
  // Dapatkan suhu
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity(); 


  uint8_t CMDID = asr.getCMDID();
    if (CMDID > 0) {
      Serial.print("CMDID terdeteksi: ");
      Serial.println(CMDID);
    }


    switch (CMDID) {
      case 1:
        dmd.clearScreen();
        lcd.clear();
        Serial.println("halo ada yang bisa di bantu?");
      case 5:  // Perintah suara "Turn on the relay"
        dmd.clearScreen();
        lcd.clear();
        lcd.print("menyalakan murattal");
        dmd.clearScreen();
        dmd.drawLine(11, 6, 15, 10);
        dmd.drawLine(15, 10, 21, 4);

        mp3_play(4);
        delay(3000);

        break;


      case 6: 
        dmd.clearScreen();
        lcd.clear(); // Perintah suara "Turn off the relay"
        lcd.print("mematikan murattal");
        dmd.clearScreen();
        dmd.drawLine(11, 6, 15, 10);
        dmd.drawLine(15, 10, 21, 4);
        mp3_stop();
        delay(3000);

        break;

      case 7: 
        dmd.clearScreen();
        lcd.clear(); // Perintah suara "Turn off the relay"
        lcd.print("menampilkan palestina");
        dmd.clearScreen();
        dmd.drawLine(11, 6, 15, 10);
        dmd.drawLine(15, 10, 21, 4);
        dmd.clearScreen();
        displayPalestine(now);
        delay(3000);
  
        break;
      
      case 8:
        dmd.clearScreen(); // Perintah suara "Turn off the relay
        lcd.clear();
        drawKalender();
        lcd.print("menampilkan kalender");
        delay(2000);
        dmd.clearScreen();
        dmd.drawLine(11, 6, 15, 10);
        dmd.drawLine(15, 10, 21, 4);
        dmd.clearScreen();
        Checkcalendar();
        setRGB(0,0,128);
        break;

      case 9:
        dmd.clearScreen(); // Perintah suara "Turn off the relay
        lcd.clear();
        lcd.print("Puzzle mulai!");
        dmd.drawString( 3,3,"Start!");
        mp3_play(6);
        delay(5000);
        dmd.clearScreen(); 
        dmd.drawCircle(16, 7, 5);
        dmd.drawFilledBox(10,0,14,6 , GRAPHICS_OFF);
        dmd.drawLine(15, 7, 15, 2);
        dmd.drawLine(15, 7, 11, 7);
        dmd.drawLine(18, 5, 18, 5);// Perintah suara "Turn off the relay
        lcd.clear();
        scrollText(pertanyaan[pIndex], 0, 200);
        pIndex = (pIndex + 1) % (sizeof(pertanyaan) / sizeof(pertanyaan[0]));
        lcd.clear();
        mp3_play(8);
        lcd.print("kamu punya");
        lcd.setCursor(0, 1);
        lcd.print("5 detik!");
        delay(3000);
        // Perintah suara "Turn off the relay
        dmd.clearScreen();
        lcd.clear();
        dmd.drawBox(14, 1, 18, 9);
        dmd.drawCircle(16,13,2);
        mp3_play(7);
        
        if (strlen(jawaban[aIndex]) > 16) {
          // Jika panjang teks lebih dari 16 karakter, gunakan scroll
          scrollText(jawaban[aIndex], 0, 200);
          aIndex = (aIndex + 1) % (sizeof(jawaban) / sizeof(jawaban[0]));
        } else {
          // Jika kurang dari atau sama dengan 16 karakter, tampilkan langsung
          lcd.print(jawaban[aIndex]);
          aIndex = (aIndex + 1) % (sizeof(jawaban) / sizeof(jawaban[0])); // Ganti `printText` dengan fungsi print yang sesuai
        }
        delay(3000);

        break;

      default:
        Serial.print("CMDID tidak valid : ");
        Serial.println(CMDID);
        break;
      
    }
  displayTimeOnP10(now);
    // Handle alarm conditions
  if (alarmOn) {
        setRGB(128, 0, 0); // Red
        if (digitalRead(PIR_PIN) == HIGH) {
            stopAlarm();
            setRGB(0, 128, 0); // Green after stopping alarm
        }
  }

  // Handle temperature-based RGB indication
  


  
  
  checkAlarm(now);
  checkPuasaSeninKamis(now);
  checkAlarmBeforeAzan(now);
  checkAdzanAlarm(now);
  checkMurattalSchedule(now);

  int currentMinute = now.Minute(); // Ambil menit saat ini
  
  // Cek jika menit berubah
  if (currentMinute != previousMinute) {
    previousMinute = currentMinute; // Update menit sebelumnya

    // Tampilkan waktu di P10
    displayTimeOnP10(now);

  }

  // Tampilkan waktu di P10



  if (alarmOn) {
    if (digitalRead(PIR_PIN) == HIGH) { // Jika PIR mendeteksi gerakan
      stopAlarm();
      displayTimeOnP10(now); 
      displayContent(displayState, now);// Matikan alarm
    }
  }
  // Rotasi tampilan di LCD
   // Rotasi tampilan di LCD
  unsigned long currentMillis = millis();
  if (currentMillis - lastDisplayChange >= displayInterval) {
    lastDisplayChange = currentMillis;
    displayState = (displayState + 1) % 7; 
    // Tambahkan satu state lagi (untuk Palestine)

    if (displayState == 6) { 
      // Jika state adalah Palestine, tampilkan bendera Palestina
      displayPalestine(now);
      delay(2000);
      displayTimeOnP10(now); // Tampilkan bendera selama 5 detik
    } else {
      // Tampilkan rotasi konten lainnya
      displayContent(displayState, now);
    }
  }

  



  // Tampilkan konten sesuai state
  

  // Jeda untuk mengurangi beban
  delay(50);
}

void displayContent(int state, RtcDateTime now) {

  int mekkahHour = (now.Hour() - 4) % 24;
  // Dapatkan suhu
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity(); 
  
  switch (displayState) {
    
   
    

    case 1:
      // Tampilkan suhu
      lcd.setCursor(0, 0);
      if (!isnan(temperature)) {
        if (temperature > 30) {
          scrollText("Walau panas ga boleh malas!", 0, 200);
          delay(3000);
          lcd.clear();
        } else if (temperature < 30){
          scrollText("lagi cerah nih, Alhamdulillah!", 0,200);
          delay(3000);
          lcd.clear();
        } else if (temperature < 23){
          lcd.print("Jangan lupa doa hujan!");
          lcd.setCursor(0, 1);
          scrollText("Allahumma shayyiban haniyyâ wa sayyiban nâfi'â", 0, 200);
          buzzInRhythm();
          delay(3000);
          lcd.clear();
          
        }
        lcd.setCursor(2, 0);
        lcd.print("Suhu: ");
        lcd.print(temperature, 1);
        lcd.print("C");
        lcd.setCursor(0, 1); // Baris kedua, kolom pertama
        lcd.print("Kelembapan: ");
        lcd.print(humidity, 1); // 1 digit desimal
        lcd.print(" %");
        delay(3000);
        lcd.clear();
      } else {
        scrollText("ups ada sedikit error!",0,200);
      }
      break;

    case 2:
      // Tampilkan Doa Harian
      scrollText(dailyDoas[doaIndex], 0, 200);
      doaIndex = (doaIndex + 1) % (sizeof(dailyDoas) / sizeof(dailyDoas[0]));
      break;

    case 3:
      // Tampilkan Hadis Harian
      scrollText(dailyHadis[hadisIndex], 0, 200);
      hadisIndex = (hadisIndex + 1) % (sizeof(dailyHadis) / sizeof(dailyHadis[0]));
      break;

    case 4:
      // Tampilkan Kutipan Islami
      scrollText(islamicQuotes[quoteIndex], 0, 200);
      quoteIndex = (quoteIndex + 1) % (sizeof(islamicQuotes) / sizeof(islamicQuotes[0]));
      break;


    case 5:
      // Tampilkan waktu sholat
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Subuh: ");
      lcd.print(formatTime(waktuSholat[0][0]));
      lcd.print(":");
      lcd.print(formatTime(waktuSholat[0][1]));
      
      lcd.setCursor(0, 1);
      lcd.print("Dzuhur: ");
      lcd.print(formatTime(waktuSholat[1][0]));
      lcd.print(":");
      lcd.print(formatTime(waktuSholat[1][1]));

      delay(3300);
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Asar: ");
      lcd.print(formatTime(waktuSholat[2][0]));
      lcd.print(":");
      lcd.print(formatTime(waktuSholat[2][1]));

      lcd.setCursor(0, 1);
      lcd.print("Maghrib: ");
      lcd.print(formatTime(waktuSholat[3][0]));
      lcd.print(":");
      lcd.print(formatTime(waktuSholat[3][1]));

      delay(3300);
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Isya: ");
      lcd.print(formatTime(waktuSholat[4][0]));
      lcd.print(":");
      lcd.print(formatTime(waktuSholat[4][1]));

      delay(3300);
      lcd.clear();
      break;




  }// Implementasi sesuai kebutuhan Anda
}

void setRGB(int red, int green, int blue) {
    analogWrite(RED_PIN, red);
    analogWrite(GREEN_PIN, green);
    analogWrite(BLUE_PIN, blue);
}

void scrollText(const char *text, int row, int delayTime) {
    lcd.clear(); // Clear display before scrolling
    int length = strlen(text);
    for (int i = 0; i < length; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(text + i);
        lcd.setCursor(0, 1);
        lcd.print("--------------------------------");
        delay(delayTime);
    }
}

String convertToHijri(int year, int month, int day) {
  // Konversi sederhana Gregorian ke Hijriah (estimasi)
  int hijriYear = year - 578;
  int hijriMonth = (month + 5) % 12 + 1;
  int hijriDay = day - 2;

  return String(hijriDay) + "/" + String(hijriMonth) + "/" + String(hijriYear);
}

void buzzInRhythm() {
  // Pola irama buzzer: hidupkan 200ms, matikan 200ms (ulang beberapa kali)
  for (int i = 0; i < 15; i++) {
    digitalWrite(BUZZER_PIN, HIGH);  // Nyalakan buzzer
    delay(200);                      // Tunggu selama 200ms
    digitalWrite(BUZZER_PIN, LOW);   // Matikan buzzer
    delay(200);                      // Tunggu selama 200ms
  }
}

void Checkcalendar() {
  RtcDateTime now = Rtc.GetDateTime();
  int year = now.Year();
  int thousands = year / 1000;  // Ambil ribuan
  int hundreds = (year % 1000) / 100;  // Ambil ratusan
  int tens = (year % 100) / 10;  // Ambil puluhan
  int ones = year % 10; 

  int mekkahHour = (now.Hour() - 4 + 24) % 24; // Ensure it wraps around correctly
  int mekkahMinute = now.Minute();
  int mekkahSecond = now.Second();

   // Konversi ke Hijriah
  String hijriDate = convertToHijri(now.Year(), now.Month(), now.Day());

  // Pisahkan tanggal Hijriah (DD/MM/YYYY) menjadi bagian-bagian
  int separatorIndex1 = hijriDate.indexOf('/');
  int separatorIndex2 = hijriDate.lastIndexOf('/');
  
  String hijriDay = hijriDate.substring(0, separatorIndex1);       // Hari
  String hijriMonth = hijriDate.substring(separatorIndex1 + 1, separatorIndex2); // Bulan
  String hijriYear = hijriDate.substring(separatorIndex2 + 1);    // Tahun

  dmd.clearScreen();
  box.clear();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tanggal hari ini:");

  // Tampilkan jam
  dmd.drawChar(3, 3, now.Day() / 10 + '0'); // Puluhan jam
  dmd.drawChar(10, 3, now.Day() % 10 + '0'); // Satuan jam
  dmd.drawLine(18, 3, 14, 11);
  // Tampilkan menit
  dmd.drawChar(20, 3, now.Month() / 10 + '0'); // Puluhan menit
  dmd.drawChar(28, 3, now.Month() % 10 + '0');
  delay(2500);
  box.clear();
  dmd.clearScreen();
   // Ambil satuan

  // Tampilkan tahun pada LED Matrix
  dmd.drawChar(3, 3, thousands + '0');  // Digit ribuan
  dmd.drawChar(10, 3, hundreds + '0');  // Digit ratusan
  dmd.drawChar(17, 3, tens + '0');  // Digit puluhan
  dmd.drawChar(24, 3, ones + '0');  // Digit satuan
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kalender Hijriah:"); // Format: DD/MM/YYYY
  

  // Tampilkan Hijri Day dan Month di P10
  dmd.clearScreen();
  for (int i = 0; i < hijriDay.length(); i++) {
    dmd.drawChar(1 + i * 8, 3, hijriDay[i]);
  }
  dmd.drawChar(hijriDay.length() * 8 + 1, 3, '/'); // Tambahkan slash
  for (int i = 0; i < hijriMonth.length(); i++) {
    dmd.drawChar((hijriDay.length() + 1) * 8 + i * 8, 3, hijriMonth[i]);
  }
  delay(3000);

  // Tampilkan Hijri Year di P10
  dmd.clearScreen();
  for (int i = 0; i < hijriYear.length(); i++) {
    dmd.drawChar(1 + i * 8, 3, hijriYear[i]);
  }
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Waktu Mekkah:");
  lcd.setCursor(0, 1);
  dmd.clearScreen();
  dmd.drawChar(1, 3, mekkahHour / 10 + '0'); // Puluhan jam
  dmd.drawChar(8, 3, mekkahHour % 10 + '0'); // Satuan jam
  dmd.drawLine(15, 4, 15, 5);
  dmd.drawLine(15, 9, 15, 10);               // Separator
  dmd.drawChar(18, 3, mekkahMinute / 10 + '0'); // Puluhan menit
  dmd.drawChar(26, 3, mekkahMinute % 10 + '0'); // Satuan menit
  delay(3000);

  // Tampilkan Waktu Mekkah di LCD
  
  setRGB(0, 128, 0);
  
}

// void checkAlarm(const RtcDateTime &now) {
//   for (int i = 0; i < alarmCount; i++) {
//     if (now.Hour() == alarmHours[i] && now.Minute() == alarmMinutes[i] && now.Second() < 10) {
//       if (!alarmOn || activeAlarmIndex != i) { // Hanya aktifkan jika belum menyala
//         alarmOn = true;
//         activeAlarmIndex = i;
//         triggerAlarm();
//         delay(5000);
//         displayTimeOnP10(now);
//       }
//       return;
//     }
//   }
//   alarmOn = false; // Matikan alarm jika tidak ada yang sesuai
// }

// void triggerAlarm() {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("ALARM: ");
//   lcd.setCursor(0, 1);
//   lcd.print(alarmMessages[activeAlarmIndex]);
//   box.clear();
//   dmd.clearScreen();
//   dmd.drawBox(14, 1, 18, 9);
//   dmd.drawCircle(16,13,2);
//   buzzInRhythm();

//   // Tampilkan tanda seru (!) di P10
//    // Titik bawah tanda seru
// }

// void stopAlarm() {
//   alarmOn = false; // Set status alarm menjadi mati
//   activeAlarmIndex = -1; // Reset alarm index

//   dmd.clearScreen();
//   // Matikan buzzer


//   digitalWrite(BUZZER_PIN, LOW);

//   // Tampilkan pesan alarm selesai
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Jangan lupa ibadah:D");
//   delay(2000); // Beri waktu untuk membaca pesan
//   lcd.clear();
// }

// Cek pengingat puasa Senin-Kamis secara terus-menerus
void checkPuasaSeninKamis(const RtcDateTime &now) {
  int currentDayOfWeek = now.DayOfWeek(); // 0: Minggu, 1: Senin, ..., 6: Sabtu

  // Pastikan hari Senin atau Kamis dan waktu berada di rentang tertentu
  if ((currentDayOfWeek == 1 || currentDayOfWeek == 4) &&
      now.Hour() == 3 && now.Minute() == 50) {
    setRGB(0, 0,128);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Puasa Sunnah!");
    lcd.setCursor(0, 1);
    lcd.print("Senin/Kamis");
    box.clear();
    dmd.clearScreen();
    dmd.drawBox(14, 1, 18, 9);
    dmd.drawCircle(16,13,2);
    buzzInRhythm(); 
    delay(5000); // Mainkan alarm
    setRGB(0, 128, 0);
  }
}

void displayPalestine(RtcDateTime now) {
  box.clear();
  dmd.clearScreen();
  lcd.clear();
  lcd.print("Jangan lupakan");
  lcd.setCursor(0, 1);
  lcd.print("PALESTINA!");
  dmd.drawLine(0, 0, 0, 15);  // Garis vertikal kiri
  dmd.drawLine(0, -1, 11, 7); // Garis diagonal ke kanan
  dmd.drawLine(11, 8, 0, 16); 
  dmd.drawLine(31, 4, 7, 4);
  dmd.drawLine(31, 11, 7, 11);
  delay(3000);
  dmd.clearScreen();
  dmd.drawCircle(16, 7, 6);
  dmd.drawLine(12, 5, 14, 7);
  dmd.drawLine(14, 5, 12, 7);
  dmd.drawLine(20, 5, 18, 7);
  dmd.drawLine(18, 5, 20, 7);
  dmd.drawLine(15, 10, 17, 10);
  lcd.clear();
  lcd.print("42000 korban!");
  delay(3000);
  lcd.clear();
  lcd.print("428 hari sejak");
  lcd.setCursor(0, 1);
  lcd.print("GENOSIDA!");
  // delay(4000);
  // box.clear();
  // dmd.clearScreen();
  // lcd.clear();


}

void displayTimeOnP10(RtcDateTime now) {
  // Menampilkan jam dan menit di P10
  setRGB(0,128,0);
  box.clear();
  dmd.clearScreen();
  // Tampilkan jam
  dmd.drawChar(1, 3, now.Hour() / 10 + '0'); // Puluhan jam
  dmd.drawChar(8, 3, now.Hour() % 10 + '0'); // Satuan jam
  dmd.drawLine(15, 4, 15, 5);
  dmd.drawLine(15, 9, 15, 10);
  
  // Tampilkan menit
  dmd.drawChar(18, 3, now.Minute() / 10 + '0'); // Puluhan menit
  dmd.drawChar(26, 3, now.Minute() % 10 + '0'); // Satuan menit

 // Perbarui tampilan P10
}

void checkAlarmBeforeAzan(const RtcDateTime &now) {
  for (int i = 0; i < 5; i++) {
    if (now.Hour() == waktuSholat[i][0] && now.Minute() == waktuSholat[i][1] - 5) {
      if (!alarmOn) { // Aktifkan alarm jika belum menyala
        alarmOn = true;
        activeAlarmIndex = i;
        triggerAlarmBeforeAzan();
      }
      return;
    }
    else if (now.Hour() == waktuSholat[0][0] && now.Minute() == waktuSholat[1][0] - 5) {
      if (!alarmOn) { // Aktifkan alarm jika belum menyala
        alarmOn = true;
        activeAlarmIndex = i;
        triggerAlarmBeforeAzan();
      }
      return;
    }
  }
  alarmOn = false; // Matikan alarm jika tidak ada yang sesuai
}

// Fungsi untuk menampilkan pesan di LCD dan gambar masjid di P10
void triggerAlarmBeforeAzan() {
  setRGB(128,0, 0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("5 Menit lagi adzan!");
  mp3_play(5);
  drawMasjid(); 
  delay(7000);
  mp3_stop(); // Gambar masjid di P10
    // Buzzer berbunyi
}
void drawMasjid() {
  box.clear();
  dmd.clearScreen();
  dmd.drawFilledBox(10, 10, 22, 16);
  dmd.drawFilledBox(16, 12, 17, 15, GRAPHICS_OFF);
  dmd.drawLine(7, 7, 10, 7);
  dmd.drawLine(22, 7, 25, 7);
  dmd.drawFilledBox(8, 6, 9, 16);
  dmd.drawFilledBox(23, 6, 24, 16);
  dmd.drawCircle(16,10,5); 
   // Kubah masjid
}

// Modify checkAlarm to properly handle alarm based on current time
void checkAlarm(RtcDateTime now) {
  int currentHour = now.Hour();
  int currentMinute = now.Minute();
  bool alarmTriggered = false;
  

  for (int i = 0; i < alarmCount; i++) {
    // Check if current time matches the alarm time
    if (currentHour == alarmHours[i] && currentMinute == alarmMinutes[i] && !alarmOn) {
      // Activate the alarm
      alarmOn = true;
      activeAlarmIndex = i;
      alarmTriggered = true;
      Serial.print("Alarm Triggered: ");
      Serial.println(alarmMessages[i]);
      startAlarm();
      break;  // No need to check other alarms once one is triggered
    }
  }

  // If motion detected while alarm is active, stop the alarm
  if (alarmOn && digitalRead(PIR_PIN) == HIGH) {
    stopAlarm();
    alarmOn = false;
    Serial.println("Alarm Stopped due to motion detection.");
  }

  // If alarm is triggered, handle it
  if (alarmTriggered) {
    // Perform actions when the alarm is triggered (e.g., sound, display messages, etc.)
    showAlarmMessage(activeAlarmIndex);
  }
}

// Start alarm sound (e.g., buzzer)
void startAlarm() { 
  box.clear();
  dmd.clearScreen();
  dmd.drawBox(14, 1, 18, 9);
  dmd.drawCircle(16,13,2);// Turn on the buzzer
  setRGB(128, 0, 0); // Set RGB to red to indicate alarm state
  buzzInRhythm();
}

// Stop the alarm sound
void stopAlarm() {
  digitalWrite(BUZZER_PIN, LOW);// Turn off the buzzer
  setRGB(0, 128, 0); // Set RGB to green after alarm stops
}

// Show message on screen when alarm triggers
void showAlarmMessage(int alarmIndex) {
  lcd.clear();
  scrollText(alarmMessages[alarmIndex],0,200);
  delay(5000); // Show the message for 3 seconds
  lcd.clear();
}

void checkAdzanAlarm(RtcDateTime now) {
  // Periksa setiap waktu adzan yang sudah ditentukan
  for (int i = 0; i < 5; i++) {
    if (now.Hour() == waktuSholat[i][0] && now.Minute() == waktuSholat[i][1]) {
      if (!alarmOn) {
        alarmOn = true;
        activeAlarmIndex = i;
        playAdzanAlarm(); // Panggil fungsi untuk memutar adzan
      }
    }
    
  }
  
  // Matikan alarm setelah beberapa detik jika sudah selesai
  
}
void playAdzanAlarm() {
  lcd.clear();
  lcd.setCursor(0, 0);
  scrollText("Sudah waktunya sholat!",0,200);
  drawMasjid();  // Gambar masjid di P10
  mp3_play(2); 
  delay(3000); // Buzzer berbunyi
}
void stopAdzanAlarm() {
  lcd.clear();
  dmd.clearScreen();
  mp3_stop();
  
}

void checkMurattalSchedule(RtcDateTime now) {
  for (int i = 0; i < murattalCount; i++) {
    if (now.Hour() == murattalHours[i] && now.Minute() == murattalMinutes[i]) {
      if (!murattalPlaying) {
        murattalPlaying = true;
        activeMurattalIndex = i;
        
        // Play the appropriate surah based on the schedule
        if (i % 2 == 0) {
          mp3_play(4); // Play Surah from file 0004 for even indices
        } else {
          mp3_play(3); // Play Surah from file 0003 for odd indices
        }
      }
    }
  }
}

// void displayQuestion(int pIndex) {
//   scrollText(pertanyaan[pIndex], 0, 300);
//   pIndex = (pIndex + 1) % (sizeof(pertanyaan) / sizeof(pertanyaan[0]));
// }

// Fungsi menampilkan jawaban
#include <string.h> // Untuk fungsi strlen

// void displayAnswer(int aIndex) {
//   if (strlen(jawaban[aIndex]) > 16) {
//     // Jika panjang teks lebih dari 16 karakter, gunakan scroll
//     scrollText(jawaban[aIndex], 0, 300);
//     aIndex = (aIndex + 1) % (sizeof(jawaban) / sizeof(jawaban[0]));
//   } else {
//     // Jika kurang dari atau sama dengan 16 karakter, tampilkan langsung
//     lcd.print(jawaban[aIndex]);
//     aIndex = (aIndex + 1) % (sizeof(jawaban) / sizeof(jawaban[0])); // Ganti `printText` dengan fungsi print yang sesuai
//   }
  
// }
void drawKalender() {
  dmd.drawFilledBox(6, 2, 24, 12);
  dmd.drawFilledBox(7, 3, 8, 3, GRAPHICS_OFF);
  dmd.drawLine(9, 5, 9, 12, GRAPHICS_OFF);
  dmd.drawLine(13, 5, 13, 12, GRAPHICS_OFF);
  dmd.drawLine(17, 5, 17, 12, GRAPHICS_OFF);
  dmd.drawLine(21, 5, 21, 12, GRAPHICS_OFF);
  dmd.drawLine(6, 5, 24, 5, GRAPHICS_OFF);
  dmd.drawLine(6, 9, 24, 9, GRAPHICS_OFF);
  dmd.drawLine(9, 0, 9, 1);
  dmd.drawLine(21, 0, 21, 1);
}




