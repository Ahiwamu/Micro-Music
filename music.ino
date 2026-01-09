#define LED 27   // ต่อกับ Piezo หรือ LED

hw_timer_t *My_timer = NULL;

// ===== โน้ตดนตรี (Octave 6) =====
#define C6 1047
#define D6 1175
#define E6 1319
#define F6 1397
#define G6 1568
#define A6 1760
#define B6 1976
#define C7 2093
#define REST 0

// ===== ทำนอง: -ซซซ ลซมซ -ล-ด =====
int melody[] = {
  REST,
  G6, G6, G6,
  A6, G6, E6, G6,
  REST,
  A6,
  REST,
  C7
};

// ===== ความยาวโน้ต (ms) =====
int noteDuration[] = {
  300,            // -
  250, 250, 300, // ซ ซ ซ
  350, 250, 250, 300, // ล ซ ม ซ
  300,            // -
  400,            // ล
  300,            // -
  600             // ด (จบวลี)
};

int totalNotes = sizeof(melody) / sizeof(melody[0]);

int noteIndex = 0;
unsigned long lastNoteTime = 0;
bool isGap = false;

// ===== Timer ISR =====
void IRAM_ATTR onTimer()
{
  digitalWrite(LED, !digitalRead(LED));
}

// ===== เปลี่ยนความถี่ตามโน้ต =====
void playNote(int freq)
{
  if (freq == REST) {
    digitalWrite(LED, LOW);
    // ตั้ง alarm ยาว ๆ เพื่อให้เงียบ
    timerAlarm(My_timer, 1000000, true, 0);
    return;
  }

  int s = 1000000 / freq / 2; // half period
  timerAlarm(My_timer, s, true, 0);
}

void setup()
{
  pinMode(LED, OUTPUT);

  My_timer = timerBegin(1000000);          // Timer 1 MHz
  timerAttachInterrupt(My_timer, &onTimer);

  // เริ่มต้นแบบเงียบ
  timerAlarm(My_timer, 1000000, true, 0);

  // เล่นโน้ตแรก
  playNote(melody[noteIndex]);
  lastNoteTime = millis();
}

void loop()
{
  unsigned long now = millis();

  if (!isGap) {
    // ===== ช่วงเล่นโน้ต =====
    if (now - lastNoteTime >= noteDuration[noteIndex]) {
      playNote(REST);          // หยุดเสียง
      lastNoteTime = now;
      isGap = true;
    }
  } else {
    // ===== ช่วงเว้นจังหวะ =====
    int gapDuration = noteDuration[noteIndex] * 0.25; // เว้น 25%

    if (now - lastNoteTime >= gapDuration) {
      noteIndex++;
      if (noteIndex >= totalNotes) {
        noteIndex = 0; // วนเพลง
      }

      playNote(melody[noteIndex]);
      lastNoteTime = now;
      isGap = false;
    }
  }
}
