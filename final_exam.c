#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 12

typedef struct Student {
    char name[MAX_NAME_LENGTH];
    int  kor;
    int  eng;
    int  math;
    int  total;
    float average;
    int  ranking;
    struct Student *p_next;
} Student;

Student* create_node(Student *src) {
    Student *p = (Student*)malloc(sizeof(Student));
    if (p == NULL) {
        printf("메모리 할당 실패\n");
        exit(1);
    }
    *p = *src;
    p->p_next = NULL;
    return p;
}

void delete_all(Student **pp_head) {
    Student *p = *pp_head, *next;
    while (p != NULL) {
        next = p->p_next;
        free(p);
        p = next;
    }
    *pp_head = NULL;
}

int get_count(Student *head) {
    int c = 0;
    while (head != NULL) {
        c++;
        head = head->p_next;
    }
    return c;
}

void list_to_array(Student *head, Student *arr) {
    int i = 0;
    while (head != NULL) {
        arr[i++] = *head;
        head = head->p_next;
    }
}

Student* array_to_list(Student *arr, int n) {
    Student *head = NULL, *tail = NULL, *p;
    for (int i = 0; i < n; i++) {
        p = (Student*)malloc(sizeof(Student));
        if (p == NULL) {
            printf("메모리 할당 실패\n");
            exit(1);
        }
        *p = arr[i];
        p->p_next = NULL;

        if (head == NULL) {
            head = tail = p;
        } else {
            tail->p_next = p;
            tail = p;
        }
    }
    return head;
}

void calc_score_and_rank(Student *head) {
    Student *p, *q;

    p = head;
    while (p != NULL) {
        p->total   = p->kor + p->eng + p->math;
        p->average = p->total / 3.0f;
        p = p->p_next;
    }

    p = head;
    while (p != NULL) {
        p->ranking = 1;
        p = p->p_next;
    }

    for (p = head; p != NULL; p = p->p_next) {
        for (q = head; q != NULL; q = q->p_next) {
            if (p->total < q->total)
                p->ranking++;
        }
    }
}

void sort_by_total_desc(Student *head) {
    if (head == NULL) return;

    int swapped;
    Student *p, *q;
    Student temp;

    do {
        swapped = 0;
        p = head;
        while (p->p_next != NULL) {
            q = p->p_next;
            if (p->total < q->total) {
                temp = *p;
                Student *p_next = p->p_next;
                Student *q_next = q->p_next;

                *p = *q;
                *q = temp;

                p->p_next = p_next;
                q->p_next = q_next;

                swapped = 1;
            }
            p = p->p_next;
        }
    } while (swapped);
}

void input_one_student(Student *s, int count) {
    printf("%d 번째 학생 이름 : ", count);
    scanf("%s", s->name);
    printf("국어 점수 : ");
    scanf("%d", &s->kor);
    printf("영어 점수 : ");
    scanf("%d", &s->eng);
    printf("수학 점수 : ");
    scanf("%d", &s->math);
    s->total   = s->kor + s->eng + s->math;
    s->average = s->total / 3.0f;
    s->ranking = 1;
}

void append_student(Student **pp_head, Student *data) {
    Student *new_node = create_node(data);
    if (*pp_head == NULL) {
        *pp_head = new_node;
    } else {
        Student *p = *pp_head;
        while (p->p_next != NULL)
            p = p->p_next;
        p->p_next = new_node;
    }
    calc_score_and_rank(*pp_head);
}

void display_students(Student *head) {
    if (head == NULL) {
        printf("학생 정보가 없습니다.\n");
        return;
    }

    printf("----------------------------------------------------------\n");
    printf(" 이름      국어   영어   수학   총점    평균  등수\n");
    printf("----------------------------------------------------------\n");

    while (head != NULL) {
        printf(" %-10s %5d %6d %6d %6d %7.1f %3d등\n",
               head->name,
               head->kor,
               head->eng,
               head->math,
               head->total,
               head->average,
               head->ranking);
        head = head->p_next;
    }
}

void load_from_file(Student **pp_head) {
    FILE *fp;
    int count;
    Student *arr = NULL;

    fp = fopen("students.dat", "rb");
    if (fp == NULL) {
        printf("파일을 열 수 없습니다(students.dat). 새로 시작합니다.\n");
        return;
    }

    if (fread(&count, sizeof(int), 1, fp) != 1) {
        printf("파일에서 데이터를 읽을 수 없습니다.\n");
        fclose(fp);
        return;
    }

    if (count <= 0) {
        fclose(fp);
        return;
    }

    arr = (Student*)malloc(sizeof(Student) * count);
    if (arr == NULL) {
        printf("메모리 할당 실패\n");
        fclose(fp);
        exit(1);
    }

    if (fread(arr, sizeof(Student), count, fp) != (size_t)count) {
        printf("학생 데이터를 모두 읽지 못했습니다.\n");
        free(arr);
        fclose(fp);
        return;
    }

    fclose(fp);

    delete_all(pp_head);
    *pp_head = array_to_list(arr, count);
    free(arr);

    calc_score_and_rank(*pp_head);

    printf("파일에서 %d명의 학생 데이터를 읽었습니다.\n", count);
}

void save_to_file(Student *head) {
    FILE *fp;
    int count = get_count(head);
    Student *arr;

    fp = fopen("students.dat", "wb");
    if (fp == NULL) {
        printf("파일을 열 수 없습니다(students.dat).\n");
        return;
    }

    fwrite(&count, sizeof(int), 1, fp);

    if (count > 0) {
        arr = (Student*)malloc(sizeof(Student) * count);
        if (arr == NULL) {
            printf("메모리 할당 실패\n");
            fclose(fp);
            exit(1);
        }
        list_to_array(head, arr);
        fwrite(arr, sizeof(Student), count, fp);
        free(arr);
    }

    fclose(fp);
    printf("%d명의 학생 데이터를 파일에 저장했습니다.\n", count);
}

int main(void) {
    Student *p_head = NULL;
    Student temp;
    int menu;
    int count = 0;

    while (1) {
        printf("\n[Menu]\n");
        printf("1. .dat 파일에서 데이터 읽기\n");
        printf("2. 추가 학생 정보 입력\n");
        printf("3. .dat 파일 저장\n");
        printf("4. 성적 확인 (총점/평균/등수)\n");
        printf("5. 종료\n");
        printf("-------------------\n");
        printf("선택(1~5): ");
        if (scanf("%d", &menu) != 1) {
            while (getchar() != '\n');
            continue;
        }
        printf("\n");

        if (menu == 5) {
            printf("프로그램을 종료합니다.\n");
            break;
        } else if (menu == 1) {
            load_from_file(&p_head);
            count = get_count(p_head);
        } else if (menu == 2) {
            input_one_student(&temp, count + 1);
            append_student(&p_head, &temp);
            count++;
        } else if (menu == 3) {
            save_to_file(p_head);
        } else if (menu == 4) {
            calc_score_and_rank(p_head);
            sort_by_total_desc(p_head);
            display_students(p_head);
        } else {
            printf("잘못된 선택입니다.\n");
        }
    }

    delete_all(&p_head);
    return 0;
}
