/* Processed by ecpg (18.0) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#line 1 "E:/C_Databases/Databases_on_C/test_sql.pgc"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* Объявление переменных для подключения к БД */
/* exec sql begin declare section */
      // имя базы данных
       // имя пользователя базы данных
      // пароль пользователя к базе данных
      // имя хоста (localhost - 127.0.0.1 или students.ami.nstu.ru)
      // номер порта - 5432 по умолчанию
      // имя схемы
      // имя пути для поиска
       // строка с connection

#line 17 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_name [ 64 ] ;
 
#line 18 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_user [ 64 ] ;
 
#line 19 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_password [ 64 ] ;
 
#line 20 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_host [ 64 ] ;
 
#line 21 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_port [ 10 ] ;
 
#line 22 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char db_schema_name [ 64 ] ;
 
#line 23 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char search_path_cmd [ 128 ] ;
 
#line 24 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char connection_string [ 256 ] ;
/* exec sql end declare section */
#line 25 "E:/C_Databases/Databases_on_C/test_sql.pgc"


/* Функция для обработки ошибок
Параметры:
@ error - название ошибки (str),
@ message - строка с ошибкой (str),
@ fatal - флаг ошибки.
*/
void ErrorHandler(const char *error, const char *message, bool fatal) {
    fprintf(stderr, "Ошибка: %s. %s\n", error ? error : "Unknown error", message ? message : "Unknown message");
    if (fatal) exit(EXIT_FAILURE);
}

/* Функция для обрезки пробелов в конце строки */
void trim_trailing_spaces(char *str) {
    if (str == NULL) return;
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

/* Функция для проверки пустой строки (только пробелы или пустая) */
int is_empty_string(const char *str, size_t size) {
    if (str == NULL) return 1;
    for (size_t i = 0; i < size && str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 0;
        }
    }
    return 1;
}

/* Функция для определения кодировки базы данных */
int detect_and_set_encoding() {
    /* exec sql begin declare section */
         
         
    
#line 62 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char server_encoding [ 64 ] ;
 
#line 63 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char client_encoding [ 64 ] ;
/* exec sql end declare section */
#line 64 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    /* Получаем кодировку сервера */
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "select pg_encoding_to_char ( encoding ) from pg_database where datname = current_database ( )", ECPGt_EOIT, 
	ECPGt_char,(server_encoding),(long)64,(long)1,(64)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);}
#line 70 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    if (sqlca.sqlcode < 0) {
        fprintf(stderr, "Ошибка получения кодировки БД: %s\n", sqlca.sqlerrm.sqlerrmc);
        return -1;
    }

    printf("Кодировка базы данных: %s\n", server_encoding);

    /* Определяем подходящую кодировку клиента */
#ifdef _WIN32
    /* Windows */
    if (strcmp(server_encoding, "UTF8") == 0) {
        /* UTF-8 БД -> используем UTF-8 или WIN1251 */
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        strcpy(client_encoding, "UTF8");
        printf("Установлена кодировка консоли: UTF-8\n");
    } else if (strcmp(server_encoding, "WIN1251") == 0) {
        /* WIN1251 БД -> используем WIN1251 */
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
        strcpy(client_encoding, "WIN1251");
        printf("Установлена кодировка консоли: CP1251\n");
    } else if (strcmp(server_encoding, "WIN866") == 0) {
        /* WIN866 БД -> используем WIN866 */
        SetConsoleCP(866);
        SetConsoleOutputCP(866);
        strcpy(client_encoding, "WIN866");
        printf("Установлена кодировка консоли: CP866\n");
    } else {
        /* Другие кодировки -> пытаемся UTF-8 */
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        strcpy(client_encoding, "UTF8");
        printf("Установлена кодировка консоли: UTF-8\n");
    }
#else
    /* Linux/Unix */
    if (strcmp(server_encoding, "UTF8") == 0) {
        strcpy(client_encoding, "UTF8");
        setlocale(LC_ALL, "ru_RU.UTF-8");
        printf("Установлена локаль: ru_RU.UTF-8\n");
    } else if (strcmp(server_encoding, "WIN1251") == 0) {
        strcpy(client_encoding, "WIN1251");
        setlocale(LC_ALL, "ru_RU.CP1251");
        printf("Установлена локаль: ru_RU.CP1251\n");
    } else {
        /* По умолчанию UTF-8 */
        strcpy(client_encoding, "UTF8");
        setlocale(LC_ALL, "ru_RU.UTF-8");
        printf("Установлена локаль: ru_RU.UTF-8 (по умолчанию)\n");
    }
#endif

    /* Устанавливаем кодировку клиента PostgreSQL */
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set CLIENT_ENCODING to $0", 
	ECPGt_char,(client_encoding),(long)64,(long)1,(64)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);}
#line 126 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        fprintf(stderr, "Ошибка установки кодировки клиента: %s\n", sqlca.sqlerrm.sqlerrmc);
        return -1;
    }

    printf("Кодировка клиента PostgreSQL: %s\n", client_encoding);
    return 0;
}

/* Прототипы функций */
/* ---------- Запрос 1 --------------
1.	Выдать число деталей, которые поставлялись поставщиками,
имеющими поставки с объёмом от 600 до 700 деталей.
*/
void query1(const char *schema_prefix);

/* ---------- Запрос 2 --------------
2.	Поменять местами цвета самой тяжёлой и самой лёгкой детали,
т. е. деталям с наибольшим весом установить цвет детали с минимальным весом,
а деталям с минимальным весом установить цвет детали с наибольшим весом.
Если цветов несколько, брать первый по алфавиту из этих цветов.
*/
void query2(const char *schema_prefix);

/* ---------- Запрос 3 --------------
3.	Найти поставщиков, имеющих поставки, вес которых составляет менее четверти наибольшего
веса поставки этого поставщика. Вывести номер поставщика, вес поставки,
четверть наибольшего веса поставки поставщика.
*/
void query3(const char *schema_prefix);

/* ---------- Запрос 4 --------------
4.	Выбрать изделия, для которых не поставлялось ни одной из деталей,
поставляемых поставщиком S4.
*/
void query4(const char *schema_prefix);

/* ---------- Запрос 5 --------------
5.	Выдать полную информацию о деталях, которые поставлялись ТОЛЬКО поставщиками,
имеющими максимальный рейтинг и число поставляемых деталей не менее 3.
Вывести номер, название, цвет, вес и город детали.
*/
void query5(const char *schema_prefix);

int main(void) {

    int encoding_choice = 0;
    printf("Choose console encoding: 1 - UTF8, 2 - WIN1251: ");
    while (scanf("%d", &encoding_choice) != 1 || (encoding_choice != 1 && encoding_choice != 2)) {
        printf("Ошибка. Введите 1 или 2: ");
        while(getchar() != '\n');
    }

#ifdef _WIN32
    if (encoding_choice == 1) system("chcp 65001 >nul");
    else system("chcp 1251 >nul");
#endif

    if (encoding_choice == 1) setlocale(LC_ALL, "ru_RU.UTF-8");
    else setlocale(LC_ALL, "Russian_Russia.1251");

    char db_name_cmd[64], db_user_cmd[64], db_password_cmd[64], db_host_cmd[64], db_port_cmd[10], db_schema_name_cmd[64];
    printf("Введите название базы данных: ");
    scanf("%63s", db_name_cmd);
    printf("\nВведите имя пользователя: ");
    scanf("%63s", db_user_cmd);
    printf("\nВведите пароль: ");
    scanf("%63s", db_password_cmd);
    printf("\nВведите имя хоста: ");
    scanf("%63s", db_host_cmd);
    printf("\nВведите номер порта: ");
    scanf("%9s", db_port_cmd);
    printf("\nВведите название схемы: ");
    scanf("%63s", db_schema_name_cmd);

    // strcpy(db_name, "students_win1251");
    // strcpy(db_user, "vladimir1");
    // strcpy(db_password, "FAST1987!");
    // strcpy(db_host, "127.0.0.1");
    // strcpy(db_port, "5432");

    // strcpy(db_name, "students");
    // strcpy(db_user, "pmi-b2702");
    // strcpy(db_password, "4ioi#ZHzE");
    // strcpy(db_host, "students.ami.nstu.ru");
    // strcpy(db_port, "5432");

    strcpy(db_name, db_name_cmd);
    strcpy(db_user, db_user_cmd);
    strcpy(db_password, db_password_cmd);
    strcpy(db_host, db_host_cmd);
    strcpy(db_port, db_port_cmd);
    strcpy(db_schema_name, db_schema_name_cmd);

    char schema_prefix[70];
    if (strlen(db_schema_name) > 0 && strcmp(db_schema_name, "-") != 0) {
        snprintf(schema_prefix, sizeof(schema_prefix), "%s", db_schema_name);
    } else {
        strcpy(schema_prefix, "");
    }

    snprintf(connection_string, sizeof(connection_string), "%s@%s:%s", db_name, db_host, db_port);

    printf("Подключение к базе данных %s...\n", connection_string);

    { ECPGconnect(__LINE__, 0, connection_string , db_user , db_password , NULL, 0); }
#line 232 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка подключения к базе данных", true);
    }
    printf("Подключение успешно.\n\n");

    /* Автоматическое определение и установка кодировки */
    if (detect_and_set_encoding() < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка настройки кодировки", false);
        { ECPGdisconnect(__LINE__, "CURRENT");}
#line 241 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        exit(EXIT_FAILURE);
    }

    printf("\nПодключение к базе данных успешно выполнено.\n");
    printf("Кодировка настроена автоматически.\n\n");

    if (strlen(db_schema_name) > 0 && strcmp(db_schema_name, "-") != 0) {
        snprintf(search_path_cmd, sizeof(search_path_cmd),
                "SET search_path TO %s, public", db_schema_name);
    } else {
        strcpy(search_path_cmd, "SET search_path TO public");
    }

    printf("Настройка search_path: %s\n", search_path_cmd);

    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, search_path_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 257 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0)
        fprintf(stderr, "⚠ Не удалось установить search_path: %s\n", sqlca.sqlerrm.sqlerrmc);
    else
        printf("search_path установлен: %s\n", search_path_cmd);

    int choice;
    do {
        /* Меню */
        printf("Лабораторная работа 3: Вариант 4 - Ямщиков Владимир и Минвалиева Тамила\n");
        printf("\nВыберите запрос для выполнения:\n");
        printf("1. Выдать число деталей, которые поставлялись поставщиками, имеющими поставки с объёмом от 600 до 700 деталей.\n");
        printf("2. Поменять местами цвета самой тяжёлой и самой лёгкой детали, т. е. деталям с наибольшим весом установить цвет детали с минимальным весом, а деталям с минимальным весом установить цвет детали с наибольшим весом. Если цветов несколько, брать первый по алфавиту из этих цветов.\n");
        printf("3. Найти поставщиков, имеющих поставки, вес которых составляет менее четверти наибольшего веса поставки этого поставщика. Вывести номер поставщика, вес поставки, четверть наибольшего веса поставки поставщика.\n");
        printf("4. Выбрать изделия, для которых не поставлялось ни одной из дета лей, поставляемых поставщиком S4.\n");
        printf("5. Выдать полную информацию о деталях, которые поставлялись ТОЛЬКО поставщиками, имеющи-ми максимальный рейтинг и число поставляемых деталей не менее 3. Вывести номер, название, цвет, вес и город детали.\n");
        printf("0. Выход.\n");
        printf("Введите ваш выбор (0-5): ");
        int var;
        char term;
        int valid_input = 0;
        while (!valid_input) {
            var = scanf("%d%c", &choice, &term);
            if (var != 2 || term != '\n') {
                printf("Ошибка: введите целое число от 0 до 5: ");
                while (getchar() != '\n');
            } else if (choice < 0 || choice > 5) {
                printf("Ошибка: число должно быть от 0 до 5. Попробуйте снова: ");
            } else {
                valid_input = 1;
            }
        }

        switch (choice) {
            case 1:
                query1(schema_prefix);
                break;
            case 2:
                query2(schema_prefix);
                break;
            case 3:
                query3(schema_prefix);
                break;
            case 4:
                query4(schema_prefix);
                break;
            case 5:
                query5(schema_prefix);
                break;
            case 0:
                printf("Завершение программы...\n");
                break;
            default:
                printf("Неверный выбор. Пожалуйста, попробуйте снова.\n");
                break;
        }
    } while (choice != 0);

    /* Отключение от базы данных */
    printf("Отключение от базы данных...\n");
    { ECPGdisconnect(__LINE__, "CURRENT");}
#line 317 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка отключения от базы данных", true);
    }
    printf("Отключение от базы данных прошло успешно.\n");

    return EXIT_SUCCESS;
}

/* ---------- Запрос 1 --------------
1.	Выдать число деталей, которые поставлялись поставщиками,
имеющими поставки с объёмом от 600 до 700 деталей.
*/
void query1(const char *schema_prefix) {
    /* Объявление переменных */
    /* exec sql begin declare section */
           
    
#line 333 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 int detail_count = 0 ;
/* exec sql end declare section */
#line 334 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    /* Начало транзакции */
    printf("Начало транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "begin work");}
#line 338 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка начала транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 341 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    if (strlen(schema_prefix) > 0) {
        /* exec sql begin declare section */
             
        
#line 346 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char schema_cmd [ 128 ] ;
/* exec sql end declare section */
#line 347 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        snprintf(schema_cmd, sizeof(schema_cmd), "SET search_path TO %s, public", schema_prefix);
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, schema_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 349 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка установки search_path", false);
            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 352 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        printf("search_path установлен: %s\n\n", schema_prefix);
    }
    printf("Транзакция начата\n\n");

    /* Выполнение запроса */
    printf("Выполнение запроса...\n");
    printf("1. Выдать число деталей, которые поставлялись поставщиками, имеющими поставки с объёмом от 600 до 700 деталей.\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "select count ( distinct spj . n_det ) from spj where spj . kol between 600 and 700", ECPGt_EOIT, 
	ECPGt_int,&(detail_count),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);}
#line 366 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка выполнения запроса", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 369 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    printf("Количество деталей: %d\n", detail_count);
    printf("\nЗапрос успешно выполнен\n\n");

    /* Завершение транзакции */
    printf("Завершение транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "commit work");}
#line 377 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка завершения транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 380 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    printf("Транзакция завершена успешно.\n");
}

/* ---------- Запрос 2 --------------
2.	Поменять местами цвета самой тяжёлой и самой лёгкой детали,
т. е. деталям с наибольшим весом установить цвет детали с минимальным весом,
а деталям с минимальным весом установить цвет детали с наибольшим весом.
Если цветов несколько, брать первый по алфавиту из этих цветов.
*/
void query2(const char *schema_prefix) {
    /* Начало транзакции */
    printf("Начало транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "begin work");}
#line 395 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка начала транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 398 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    if (strlen(schema_prefix) > 0) {
        /* exec sql begin declare section */
             
        
#line 403 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char schema_cmd [ 128 ] ;
/* exec sql end declare section */
#line 404 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        snprintf(schema_cmd, sizeof(schema_cmd), "SET search_path TO %s, public", schema_prefix);
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, schema_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 406 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка установки search_path", false);
            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 409 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        printf("search_path установлен: %s\n\n", schema_prefix);
    }
    printf("Транзакция начата\n\n");

    /* Выполнение UPDATE запроса для обмена весов деталей */
    printf("Выполнение запроса...\n");
    printf("2.	Поменять местами цвета самой тяжёлой и самой лёгкой детали, т. е. деталям с наибольшим весом установить цвет детали с минимальным весом, а деталям с минимальным весом установить цвет детали с наибольшим весом. Если цветов несколько, брать первый по алфавиту из этих цветов.\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "update p set cvet = case when p . ves = ( select MAX ( ves ) from p p5 ) then ( select p3 . cvet cvet1 from p p3 order by p3 . ves , p3 . cvet limit 1 ) else ( select p4 . cvet cvet2 from p p4 order by p4 . ves desc , p4 . cvet limit 1 ) end where p . ves = ( select MIN ( ves ) from p p1 ) or p . ves = ( select MAX ( ves ) from p p2 )", ECPGt_EOIT, ECPGt_EORT);}
#line 437 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode == 0) {
        printf("Вес деталей успешно обновлён.\n");
        printf("Количество обновлённых строк: %ld\n", sqlca.sqlerrd[2]);
        printf("\nЗапрос успешно выполнен\n\n");
    } else {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при обновлении весов", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 444 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }

    /* Завершение транзакции */
    printf("Завершение транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "commit work");}
#line 451 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка завершения транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 454 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    printf("Транзакция завершена успешно.\n");
}

/* ---------- Запрос 3 --------------
3.	Найти поставщиков, имеющих поставки, вес которых составляет менее четверти наибольшего
веса поставки этого поставщика. Вывести номер поставщика, вес поставки,
четверть наибольшего веса поставки поставщика.
*/
void query3(const char *schema_prefix) {
    /* Объявление переменных */
    /* exec sql begin declare section */
         
         
         
                 
    
#line 468 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char n_post [ 7 ] ;
 
#line 469 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 int kol ;
 
#line 470 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 float quarter ;
 
#line 471 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 short n_post_ind = 0 , kol_ind = 0 , quarter_ind = 0 ;
/* exec sql end declare section */
#line 472 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    char n_post_display[25];
    char kol_display[25];

    /* Начало транзакции */
    printf("Начало транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "begin work");}
#line 479 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка начала транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 482 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    if (strlen(schema_prefix) > 0) {
        /* exec sql begin declare section */
             
        
#line 487 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char schema_cmd [ 128 ] ;
/* exec sql end declare section */
#line 488 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        snprintf(schema_cmd, sizeof(schema_cmd), "SET search_path TO %s, public", schema_prefix);
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, schema_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 490 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка установки search_path", false);
            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 493 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        printf("search_path установлен: %s\n\n", schema_prefix);
    }
    printf("Транзакция начата\n\n");

    /* Открытие курсора */
    printf("Открытие курсора...\n");
    printf("Выполнение запроса...\n");
    printf("3.	Найти поставщиков, имеющих поставки, вес которых составляет менее четверти наибольшего веса поставки этого поставщика. Вывести номер поставщика, вес поставки, четверть наибольшего веса поставки поставщика.\n");
    /* declare parts_cursor1 cursor for select distinct spj . n_post , spj . kol , max_kol . quarter as quarter_max from spj left join ( select n_post , MAX ( kol ) / 4.0 as quarter from spj group by n_post ) max_kol on spj . n_post = max_kol . n_post where spj . kol < max_kol . quarter order by spj . n_post , spj . kol asc */
#line 513 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    /* Проверка успешности определения курсора */
    if (sqlca.sqlcode < 0){
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось определить курсор", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 517 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }

    printf("Курсор определен\n\n");

    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "declare parts_cursor1 cursor for select distinct spj . n_post , spj . kol , max_kol . quarter as quarter_max from spj left join ( select n_post , MAX ( kol ) / 4.0 as quarter from spj group by n_post ) max_kol on spj . n_post = max_kol . n_post where spj . kol < max_kol . quarter order by spj . n_post , spj . kol asc", ECPGt_EOIT, ECPGt_EORT);}
#line 524 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка открытия курсора", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 527 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    printf("Курсор открыт\n\n");

    /* Вывод результатов */
    printf("Результат запроса:\n\n");
    printf("\nПоставщики с поставками, вес которых составляет менее четверти наибольшего веса поставки этого поставщика:\n");

    /* Первый FETCH для проверки наличия данных */
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch parts_cursor1", ECPGt_EOIT, 
	ECPGt_char,(n_post),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_post_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_int,&(kol),(long)1,(long)1,sizeof(int), 
	ECPGt_short,&(kol_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_float,&(quarter),(long)1,(long)1,sizeof(float), 
	ECPGt_short,&(quarter_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 538 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode == 100) {
        printf("Данных не найдено.\n");
    } else if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
        printf("Закрытие курсора...\n");
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor1", ECPGt_EOIT, ECPGt_EORT);}
#line 544 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось закрыть курсор", false);
        } else {
            printf("Курсор закрыт\n");
        }
        printf("Транзакция отменена\n\n");
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 551 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    } else {
        /* Есть данные - выводим заголовок и первую строку */
        printf("--------------------------------------------------------------------------\n");
        printf("%-9s | %-10s | %-25s\n", "Поставщик", "Вес", "Четверть макс. веса");
        printf("--------------------------------------------------------------------------\n");
        if (n_post_ind < 0 || is_empty_string(n_post, sizeof(n_post)))
            strcpy(n_post_display, "Отсутствует");
        else
            strncpy(n_post_display, n_post, sizeof(n_post_display)-1);
            n_post_display[sizeof(n_post_display)-1] = '\0';
            trim_trailing_spaces(n_post_display);

        if (kol_ind < 0)
            strcpy(kol_display, "Отсутствует");
        else
            snprintf(kol_display, sizeof(kol_display), "%d", kol);
        printf("%-9s | %-8s | %-20.2f\n", n_post_display, kol_display, quarter);
    }

    /* Цикл для остальных строк */
    while (1) {
        /* Очистка переменных перед каждым FETCH */
        memset(n_post, 0, sizeof(n_post));
        kol = 0;
        quarter = 0.0;
        n_post_ind = kol_ind = quarter_ind = 0;

        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch parts_cursor1", ECPGt_EOIT, 
	ECPGt_char,(n_post),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_post_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_int,&(kol),(long)1,(long)1,sizeof(int), 
	ECPGt_short,&(kol_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_float,&(quarter),(long)1,(long)1,sizeof(float), 
	ECPGt_short,&(quarter_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 580 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode == 100) {
            break;
        } else if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
            { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor1", ECPGt_EOIT, ECPGt_EORT);}
#line 585 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 586 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        if (n_post_ind < 0 || is_empty_string(n_post, sizeof(n_post)))
            strcpy(n_post_display, "Отсутствует");
        else
            strncpy(n_post_display, n_post, sizeof(n_post_display)-1);
            n_post_display[sizeof(n_post_display)-1] = '\0';
            trim_trailing_spaces(n_post_display);

        if (kol_ind < 0)
            strcpy(kol_display, "Отсутствует");
        else
            snprintf(kol_display, sizeof(kol_display), "%d", kol);
        printf("%-9s | %-8s | %-20.2f\n", n_post_display, kol_display, quarter);
    }

    printf("\nЗапрос успешно выполнен\n\n");

    /* Закрытие курсора */
    printf("Закрытие курсора...\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor1", ECPGt_EOIT, ECPGt_EORT);}
#line 607 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    /* Проверка, успешно ли закрылся курсор */
    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось закрыть курсор", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 611 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    printf("Курсор закрыт\n\n");

    /* Завершение транзакции */
    printf("Завершение транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "commit work");}
#line 619 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка завершения транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 622 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    printf("Транзакция завершена успешно.\n");
}

/* ---------- Запрос 4 --------------
4.	Выбрать изделия, для которых не поставлялось ни одной из деталей,
поставляемых поставщиком S4.
*/
void query4(const char *schema_prefix) {
    /* Объявление переменных */
    /* exec sql begin declare section */
         
         
         
                 
    
#line 635 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char n_izd [ 7 ] ;
 
#line 636 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char name [ 21 ] ;
 
#line 637 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char town [ 21 ] ;
 
#line 638 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 short n_izd_ind = 0 , name_ind = 0 , town_ind = 0 ;
/* exec sql end declare section */
#line 639 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    /* Вспомогательные буферы для безопасной обработки NULL */
    char n_izd_display[25];
    char name_display[25];
    char town_display[25];

    /* Начало транзакции */
    printf("Начало транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "begin work");}
#line 648 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка начала транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 651 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    }
    if (strlen(schema_prefix) > 0) {
        /* exec sql begin declare section */
             
        
#line 656 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char schema_cmd [ 128 ] ;
/* exec sql end declare section */
#line 657 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        snprintf(schema_cmd, sizeof(schema_cmd), "SET search_path TO %s, public", schema_prefix);
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, schema_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 659 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка установки search_path", false);
            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 662 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        printf("search_path установлен: %s\n\n", schema_prefix);
    }
    printf("Транзакция начата\n\n");

    /* Открытие курсора */
    printf("Открытие курсора...\n");
    printf("Выполнение запроса...\n");
    printf("4.	Выбрать изделия, для которых не поставлялось ни одной из дета лей, поставляемых поставщиком S4.\n");
    /* declare supplier_cursor cursor for select j . n_izd , j . name , j . town from j except select j1 . n_izd , j1 . name , j1 . town from spj t join j j1 on j1 . n_izd = t . n_izd where t . n_det in ( select n_det from spj a where a . n_post = 'S4' ) order by n_izd asc */
#line 685 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    /* Проверка успешности определения курсора */
    if (sqlca.sqlcode < 0){
       ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось определить курсор", false);
       { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 689 "E:/C_Databases/Databases_on_C/test_sql.pgc"

       printf("Транзакция отменена\n\n");
       return;
    }

    printf("Курсор определен.\n\n");

    /* Открытие курсора */
    printf("Открытие курсора...\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "declare supplier_cursor cursor for select j . n_izd , j . name , j . town from j except select j1 . n_izd , j1 . name , j1 . town from spj t join j j1 on j1 . n_izd = t . n_izd where t . n_det in ( select n_det from spj a where a . n_post = 'S4' ) order by n_izd asc", ECPGt_EOIT, ECPGt_EORT);}
#line 698 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    /* Проверка успешности открытия курсора */
    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка открытия курсора", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 702 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    printf("Курсор открыт\n\n");

    /* Вывод результатов */
    printf("Результат запроса:\n\n");
    printf("\nПоставщики, не поставлявшие детали для изделий из Парижа:\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch supplier_cursor", ECPGt_EOIT, 
	ECPGt_char,(n_izd),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_izd_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(name),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(name_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(town),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(town_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 711 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode == 100) {
        printf("Данных не найдено.\n");
    } else if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
        printf("Закрытие курсора...\n");
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close supplier_cursor", ECPGt_EOIT, ECPGt_EORT);}
#line 717 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось закрыть курсор", false);
        }
        else {
            printf("Курсор закрыт\n");
        }
        printf("Транзакция отменена\n\n");
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 725 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    } else {
        printf("--------------------------------------------------------------------------\n");
        printf("%-7s | %-20s | %-20s\n", "n_izd", "Название", "Город");
        printf("--------------------------------------------------------------------------\n");
        /* Безопасная обработка NULL-значений */
        if (n_izd_ind < 0 || is_empty_string(n_izd, sizeof(n_izd)))
            strcpy(n_izd_display, "Отсутствует");
        else
            strncpy(n_izd_display, n_izd, sizeof(n_izd_display)-1);
            n_izd_display[sizeof(n_izd_display)-1] = '\0';
            trim_trailing_spaces(n_izd_display);


        if (town_ind < 0 || is_empty_string(town, sizeof(town)))
            strcpy(town_display, "Отсутствует");
        else
            strncpy(town_display, town, sizeof(town_display)-1);
            town_display[sizeof(town_display)-1] = '\0';
            trim_trailing_spaces(town_display);
        printf("%-7s | %-20s | %-20s\n", n_izd_display,
           name,
           town_display);
    }
    while (1) {
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch supplier_cursor", ECPGt_EOIT, 
	ECPGt_char,(n_izd),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_izd_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(name),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(name_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(town),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(town_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 751 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode == 100) {
            break;
        } else if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
            { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close supplier_cursor", ECPGt_EOIT, ECPGt_EORT);}
#line 756 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 757 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        /* Безопасная обработка NULL-значений */
        if (n_izd_ind < 0 || is_empty_string(n_izd, sizeof(n_izd)))
            strcpy(n_izd_display, "Отсутствует");
        else
            strncpy(n_izd_display, n_izd, sizeof(n_izd_display)-1);
            n_izd_display[sizeof(n_izd_display)-1] = '\0';
            trim_trailing_spaces(n_izd_display);

        if (name_ind < 0 || is_empty_string(name, sizeof(name)))
            strcpy(name_display, "Отсутствует");
        else
            strncpy(name_display, name, sizeof(name_display)-1);
            name_display[sizeof(name_display)-1] = '\0';
            trim_trailing_spaces(name_display);

        if (town_ind < 0 || is_empty_string(town, sizeof(town)))
            strcpy(town_display, "Отсутствует");
        else
            strncpy(town_display, town, sizeof(town_display)-1);
            town_display[sizeof(town_display)-1] = '\0';
            trim_trailing_spaces(town_display);
        printf("%-7s | %-20s | %-20s\n", n_izd_display,
           name_display,
           town_display);
    }
    printf("\nЗапрос успешно выполнен\n\n");

    /* Закрытие курсора */
    printf("Закрытие курсора...\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close supplier_cursor", ECPGt_EOIT, ECPGt_EORT);}
#line 789 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка закрытия курсора", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 792 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }

    printf("Курсор закрыт\n\n");

    /* Завершение транзакции */
    printf("Завершение транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "commit work");}
#line 801 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка завершения транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 804 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    printf("Транзакция завершена успешно.\n");
}

/* ---------- Запрос 5 --------------
5.	Выдать полную информацию о деталях, которые поставлялись ТОЛЬКО поставщиками,
имеющими максимальный рейтинг и число поставляемых деталей не менее 3.
Вывести номер, название, цвет, вес и город детали.
*/
/* ---------- Запрос 5 --------------
5.	Выдать полную информацию о деталях, которые поставлялись ТОЛЬКО поставщиками,
имеющими максимальный рейтинг и число поставляемых деталей не менее 3.
Вывести номер, название, цвет, вес и город детали.
*/
void query5(const char *schema_prefix) {
    /* Объявление переменных */
    /* exec sql begin declare section */
         
         
         
         
         
                       
    
#line 824 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char n_det [ 7 ] ;
 
#line 825 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char name [ 21 ] ;
 
#line 826 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char cvet [ 21 ] ;
 
#line 827 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 int ves ;
 
#line 828 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char town [ 21 ] ;
 
#line 829 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 short n_det_ind = 0 , name_ind = 0 , cvet_ind = 0 , ves_ind = 0 , town_ind = 0 ;
/* exec sql end declare section */
#line 830 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    /* Вспомогательные буферы для безопасной обработки NULL */
    char n_det_display[25];
    char name_display[25];
    char cvet_display[25];
    char ves_display[25];
    char town_display[25];

    /* Начало транзакции */
    printf("Начало транзакции...\n");
    { ECPGtrans(__LINE__, NULL, "begin work");}
#line 841 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка начала транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 844 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    if (strlen(schema_prefix) > 0) {
        /* exec sql begin declare section */
             
        
#line 850 "E:/C_Databases/Databases_on_C/test_sql.pgc"
 char schema_cmd [ 128 ] ;
/* exec sql end declare section */
#line 851 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        snprintf(schema_cmd, sizeof(schema_cmd), "SET search_path TO %s, public", schema_prefix);
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_exec_immediate, schema_cmd, ECPGt_EOIT, ECPGt_EORT);}
#line 853 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка установки search_path", false);
            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 856 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }
        printf("search_path установлен: %s\n\n", schema_prefix);
    }
    printf("Транзакция начата\n\n");

    /* Открытие курсора */
    printf("Открытие курсора...\n");
    printf("Выполнение запроса...\n");
    printf("5. Выдать полную информацию о деталях, которые поставлялись ТОЛЬКО поставщиками, имеющими максимальный рейтинг и число поставляемых деталей не менее 3. Вывести номер, название, цвет, вес и город детали.\n");

    /* declare parts_cursor2 cursor for select p . n_det , p . name , p . cvet , p . ves , p . town from p where p . n_det in ( select distinct spj . n_det from spj where spj . n_post in ( select n_post from s where reiting = ( select MAX ( reiting ) from s ) intersect select n_post from spj group by n_post having count ( distinct n_det ) >= 3 ) except select distinct spj . n_det from spj where spj . n_post not in ( select n_post from s where reiting = ( select MAX ( reiting ) from s ) ) ) order by p . n_det asc */
#line 893 "E:/C_Databases/Databases_on_C/test_sql.pgc"


    /* Проверка успешности определения курсора */
    if (sqlca.sqlcode < 0){
       ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось определить курсор", false);
       { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 898 "E:/C_Databases/Databases_on_C/test_sql.pgc"

       printf("Транзакция отменена\n\n");
       return;
    }

    printf("Курсор определен\n\n");

    /* Открытие курсора */
    printf("Открытие курсора...\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "declare parts_cursor2 cursor for select p . n_det , p . name , p . cvet , p . ves , p . town from p where p . n_det in ( select distinct spj . n_det from spj where spj . n_post in ( select n_post from s where reiting = ( select MAX ( reiting ) from s ) intersect select n_post from spj group by n_post having count ( distinct n_det ) >= 3 ) except select distinct spj . n_det from spj where spj . n_post not in ( select n_post from s where reiting = ( select MAX ( reiting ) from s ) ) ) order by p . n_det asc", ECPGt_EOIT, ECPGt_EORT);}
#line 907 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка открытия курсора", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 910 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }

    printf("Курсор открыт\n\n");

    /* Вывод результатов */
    printf("Результат запроса:\n\n");
    printf("\nИнформация о деталях, которые поставлялись ТОЛЬКО поставщиками, имеющими максимальный рейтинг и число поставляемых деталей не менее 3:\n");

    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch parts_cursor2", ECPGt_EOIT, 
	ECPGt_char,(n_det),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_det_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(name),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(name_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(cvet),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(cvet_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_int,&(ves),(long)1,(long)1,sizeof(int), 
	ECPGt_short,&(ves_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(town),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(town_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 921 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode == 100) {
        printf("Данных не найдено.\n");
    } else if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
        printf("Закрытие курсора...\n");
        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor2", ECPGt_EOIT, ECPGt_EORT);}
#line 927 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Не удалось закрыть курсор", false);
        }
        else {
            printf("Курсор закрыт\n");
        }
        printf("Транзакция отменена\n\n");
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 935 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        return;
    } else {
        printf("------------------------------------------------------------\n");
        printf("%-7s | %-20s | %-20s | %-5s | %-20s\n", "n_det", "Название", "Цвет", "Вес", "Город");
        printf("------------------------------------------------------------\n");

        /* Безопасная обработка NULL-значений */
        if (n_det_ind < 0 || is_empty_string(n_det, sizeof(n_det)))
            strcpy(n_det_display, "Отсутствует");
        else
            strncpy(n_det_display, n_det, sizeof(n_det_display)-1);
            n_det_display[sizeof(n_det_display)-1] = '\0';
            trim_trailing_spaces(n_det_display);

        if (name_ind < 0 || is_empty_string(name, sizeof(name)))
            strcpy(name_display, "Отсутствует");
        else
            strncpy(name_display, name, sizeof(name_display)-1);
            name_display[sizeof(name_display)-1] = '\0';
            trim_trailing_spaces(name_display);

        if (cvet_ind < 0 || is_empty_string(cvet, sizeof(cvet)))
            strcpy(cvet_display, "Отсутствует");
        else
            strncpy(cvet_display, cvet, sizeof(cvet_display)-1);
            cvet_display[sizeof(cvet_display)-1] = '\0';
            trim_trailing_spaces(cvet_display);

        if (ves_ind < 0)
            strcpy(ves_display, "Отсутствует");
        else
            snprintf(ves_display, sizeof(ves_display), "%d", ves);

        if (town_ind < 0 || is_empty_string(town, sizeof(town)))
            strcpy(town_display, "Отсутствует");
        else
            strncpy(town_display, town, sizeof(town_display)-1);
            town_display[sizeof(town_display)-1] = '\0';
            trim_trailing_spaces(town_display);

        printf("%-7s | %-20s | %-15s | %-5s | %-20s\n",
               n_det_display, name_display, cvet_display, ves_display, town_display);
    }

    while (1) {
        /* Очистка буферов перед каждым FETCH */
        memset(n_det, 0, sizeof(n_det));
        memset(name, 0, sizeof(name));
        memset(cvet, 0, sizeof(cvet));
        memset(town, 0, sizeof(town));
        ves = 0;
        n_det_ind = name_ind = cvet_ind = ves_ind = town_ind = 0;

        { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "fetch parts_cursor2", ECPGt_EOIT, 
	ECPGt_char,(n_det),(long)7,(long)1,(7)*sizeof(char), 
	ECPGt_short,&(n_det_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(name),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(name_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(cvet),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(cvet_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_int,&(ves),(long)1,(long)1,sizeof(int), 
	ECPGt_short,&(ves_ind),(long)1,(long)1,sizeof(short), 
	ECPGt_char,(town),(long)21,(long)1,(21)*sizeof(char), 
	ECPGt_short,&(town_ind),(long)1,(long)1,sizeof(short), ECPGt_EORT);}
#line 989 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        if (sqlca.sqlcode == 100) {
            break;
        } else if (sqlca.sqlcode < 0) {
            ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка при получении данных", false);
            { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor2", ECPGt_EOIT, ECPGt_EORT);}
#line 994 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 995 "E:/C_Databases/Databases_on_C/test_sql.pgc"

            return;
        }

        /* Безопасная обработка NULL-значений для каждой строки */
        if (n_det_ind < 0 || is_empty_string(n_det, sizeof(n_det)))
            strcpy(n_det_display, "Отсутствует");
        else {
            strncpy(n_det_display, n_det, sizeof(n_det_display)-1);
            n_det_display[sizeof(n_det_display)-1] = '\0';
            trim_trailing_spaces(n_det_display);
        }

        if (name_ind < 0 || is_empty_string(name, sizeof(name)))
            strcpy(name_display, "Отсутствует");
        else {
            strncpy(name_display, name, sizeof(name_display)-1);
            name_display[sizeof(name_display)-1] = '\0';
            trim_trailing_spaces(name_display);
        }

        if (cvet_ind < 0 || is_empty_string(cvet, sizeof(cvet)))
            strcpy(cvet_display, "Отсутствует");
        else {
            strncpy(cvet_display, cvet, sizeof(cvet_display)-1);
            cvet_display[sizeof(cvet_display)-1] = '\0';
            trim_trailing_spaces(cvet_display);
        }

        if (ves_ind < 0)
            strcpy(ves_display, "Отсутствует");
        else
            snprintf(ves_display, sizeof(ves_display), "%d", ves);

        if (town_ind < 0 || is_empty_string(town, sizeof(town)))
            strcpy(town_display, "Отсутствует");
        else {
            strncpy(town_display, town, sizeof(town_display)-1);
            town_display[sizeof(town_display)-1] = '\0';
            trim_trailing_spaces(town_display);
        }

        printf("%-7s | %-20s | %-15s | %-5s | %-20s\n",
               n_det_display, name_display, cvet_display, ves_display, town_display);
    }

    printf("\nЗапрос успешно выполнен\n\n");

    /* Закрытие курсора */
    printf("Закрытие курсора...\n");
    { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "close parts_cursor2", ECPGt_EOIT, ECPGt_EORT);}
#line 1045 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка закрытия курсора", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 1048 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }

    /* Завершение транзакции */
    { ECPGtrans(__LINE__, NULL, "commit work");}
#line 1054 "E:/C_Databases/Databases_on_C/test_sql.pgc"

    if (sqlca.sqlcode < 0) {
        ErrorHandler(sqlca.sqlerrm.sqlerrmc, "Ошибка завершения транзакции", false);
        { ECPGtrans(__LINE__, NULL, "rollback work");}
#line 1057 "E:/C_Databases/Databases_on_C/test_sql.pgc"

        printf("Транзакция отменена\n\n");
        return;
    }
    printf("Транзакция завершена успешно.\n");
}