#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STAT_MAX       10U
#define LOW_WARNING    3U
#define NAME_LEN       12U
/* mood bits */
#define BIT_ASLEEP     0U
#define BIT_HUNGRY     1U
#define BIT_SAD        2U
#define BIT_SICK       3U

#define SET_BIT(reg, n)    ((reg) |=  (uint8_t)(1U << (n)))
#define CLR_BIT(reg, n)    ((reg) &= (uint8_t)~(1U << (n)))
#define READ_BIT(reg, n)   ((uint8_t)(((reg) >> (n)) & 1U))

typedef struct {
    char     name[NAME_LEN];
    uint8_t  food;     /* 0..10 */
    uint8_t  fun;      /* 0..10 */
    uint8_t  energy;   /* 0..10 */
    uint8_t  mood;     /* the bits above  */
    uint16_t hours;    /* how long Fluffy has been alive */
} Pet_t;

static Pet_t fluffy;

static void clearBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

static void hatchPet(void) {
    snprintf(fluffy.name, NAME_LEN, "Fluffy");
    fluffy.food = STAT_MAX;
    fluffy.fun = STAT_MAX;
    fluffy.energy = STAT_MAX;
    fluffy.mood = 0U;
    fluffy.hours = 0U;
}

static void drawStat(const char *label, uint8_t value) {
    printf("%-7s [", label);
    for (uint8_t i = 0U; i < STAT_MAX; ++i) {
        if (i < value) {
            putchar('#');
        } else {
            putchar('-');
        }
    }
    printf("] %u/%u\n", value, STAT_MAX);
}

static void drawFace(void) {
    printf("\n+-------------------+\n");
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("|     ( -_- )       |  (Sleeping)\n");
    } else if (READ_BIT(fluffy.mood, BIT_SICK)) {
        printf("|     ( x_x )       |  (Sick)\n");
    } else if (READ_BIT(fluffy.mood, BIT_SAD)) {
        printf("|     ( T_T )       |  (Sad)\n");
    } else if (READ_BIT(fluffy.mood, BIT_HUNGRY)) {
        printf("|     ( o_O )       |  (Hungry)\n");
    } else {
        printf("|     ( ^_^ )       |  (Happy)\n");
    }
    printf("+-------------------+\n");
}

static void feed(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Fluffy is asleep! Wake it up before feeding.\n");
        return;
    }
    if (fluffy.food == STAT_MAX) {
        printf("Fluffy is already full and overate! Fluffy is now SICK!\n");
        SET_BIT(fluffy.mood, BIT_SICK);
        return;
    }
    if (fluffy.food <= STAT_MAX - 3U) {
        fluffy.food += 3U;
    } else {
        fluffy.food = STAT_MAX;
    }
    printf("Yum! Fluffy ate some food.\n");
}

static void play(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        printf("Fluffy is asleep! Let it rest.\n");
        return;
    }
    if (fluffy.fun <= STAT_MAX - 3U) {
        fluffy.fun += 3U;
    } else {
        fluffy.fun = STAT_MAX;
    }

    if (fluffy.energy >= 2U) {
        fluffy.energy -= 2U;
    } else {
        fluffy.energy = 0U;
    }
    printf("Whee! Fluffy had fun playing.\n");
}

static void sleepPet(void) {
    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        CLR_BIT(fluffy.mood, BIT_ASLEEP);
        printf("Fluffy woke up!\n");
    } else {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
        printf("Fluffy went to sleep.\n");
    }
}

static void updateMood(void) {
    /* Low food means hungry, else clear */
    if (fluffy.food <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_HUNGRY);
    } else {
        CLR_BIT(fluffy.mood, BIT_HUNGRY);
    }

    /* Low fun means sad, else clear */
    if (fluffy.fun <= LOW_WARNING) {
        SET_BIT(fluffy.mood, BIT_SAD);
    } else {
        CLR_BIT(fluffy.mood, BIT_SAD);
    }

    /* No energy at all means Fluffy falls asleep */
    if (fluffy.energy == 0U) {
        SET_BIT(fluffy.mood, BIT_ASLEEP);
    }

    /* Sick only goes away after Fluffy has slept */
    if (READ_BIT(fluffy.mood, BIT_ASLEEP) && READ_BIT(fluffy.mood, BIT_SICK)) {
        CLR_BIT(fluffy.mood, BIT_SICK);
    }
}

static void hourPasses(void) {
    if (fluffy.food > 0U) {
        fluffy.food--;
    }
    if (fluffy.fun > 0U) {
        fluffy.fun--;
    }

    if (READ_BIT(fluffy.mood, BIT_ASLEEP)) {
        if (fluffy.energy <= STAT_MAX - 2U) {
            fluffy.energy += 2U;
        } else {
            fluffy.energy = STAT_MAX;
        }
    } else {
        if (fluffy.energy > 0U) {
            fluffy.energy--;
        }
    }

    if (fluffy.hours < 0xFFFFU) {
        fluffy.hours++;
    }

    updateMood();
    printf("One hour passed in Fluffy's world.\n");
}

static uint8_t isHappy(void) {
    if (fluffy.food > LOW_WARNING &&
        fluffy.fun > LOW_WARNING &&
        fluffy.energy > LOW_WARNING &&
        fluffy.mood == 0U) {
        return 1U;
    }
    return 0U;
}

static void petReport(void) {
    printf("\n========== PET REPORT ==========\n");
    printf("Name       : %s\n", fluffy.name);
    printf("Hours Alive: %u\n", fluffy.hours);
    drawStat("Food", fluffy.food);
    drawStat("Fun", fluffy.fun);
    drawStat("Energy", fluffy.energy);
    
    printf("Mood Byte (Binary): ");
    for (int i = 7; i >= 0; i--) {
        printf("%d", (fluffy.mood >> i) & 1);
    }
    printf("\nMood Hex   : 0x%02X\n", fluffy.mood);
    
    printf("Status     : ");
    if (isHappy()) {
        printf("Fluffy is very happy!\n");
    } else {
        if (READ_BIT(fluffy.mood, BIT_HUNGRY)) printf("[Needs food!] ");
        if (READ_BIT(fluffy.mood, BIT_SAD)) printf("[Needs to play!] ");
        if (READ_BIT(fluffy.mood, BIT_ASLEEP)) printf("[Is sleeping.] ");
        if (READ_BIT(fluffy.mood, BIT_SICK)) printf("[Is sick and needs rest!] ");
        printf("\n");
    }
    printf("================================\n");
}

int main(void) {
    int choice = 0;
    hatchPet();

    do {
        drawFace();
        drawStat("Food", fluffy.food);
        drawStat("Fun", fluffy.fun);
        drawStat("Energy", fluffy.energy);
        printf("Hours: %u | Happy: %s\n", fluffy.hours, isHappy() ? "Yes" : "No");

        printf("\n--- Fluffy Menu ---\n");
        printf("1. Feed Fluffy\n");
        printf("2. Play with Fluffy\n");
        printf("3. Put to Sleep / Wake Up\n");
        printf("4. Let 1 Hour Pass\n");
        printf("5. Show Pet Report\n");
        printf("6. Hatch New Pet (Reset)\n");
        printf("0. Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1: feed(); break;
            case 2: play(); break;
            case 3: sleepPet(); break;
            case 4: hourPasses(); break;
            case 5: petReport(); break;
            case 6: hatchPet(); printf("New pet hatched!\n"); break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Unknown option!\n"); break;
        }
    } while (choice != 0);

    return 0;
}