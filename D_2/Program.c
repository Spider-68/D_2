#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <limits.h>

#define MAX_V 20
#define MAX_E 50
#define INF 999999

const char* default_names[10] = {
    "", "Офіс", "Пункт 1", "Пункт 2", "Пункт 3",
    "Пункт 4", "Пункт 5", "Пункт 6", "Пункт 7", "Пункт 8"
};

int getValidInt(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf_s("%d", &value) != 1) {
            printf("  [Помилка] Потрібно ввести ціле число! Спробуйте ще раз.\n");
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        else if (value < min || value > max) {
            printf("  [Помилка] Число має бути в межах від %d до %d! Спробуйте ще раз.\n", min, max);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            return value;
        }
    }
}

void getVertexName(int index, char* buffer, size_t size) {
    if (index >= 1 && index <= 9) {
        sprintf_s(buffer, size, "%s", default_names[index]);
    }
    else {
        sprintf_s(buffer, size, "Пункт %d", index - 1);
    }
}

void printAdjacencyMatrix(int n, int A[MAX_V][MAX_V]) {
    printf("\n--- Матриця суміжності A (%dx%d) ---\n    ", n, n);
    for (int j = 1; j <= n; j++) printf("%3d", j);
    printf("\n");
    for (int i = 1; i <= n; i++) {
        printf("%2d |", i);
        for (int j = 1; j <= n; j++) {
            printf("%3d", A[i][j]);
        }
        printf("\n");
    }
}

void printReachabilityMatrix(int n, int D[MAX_V][MAX_V]) {
    printf("\n--- Матриця досяжності D (%dx%d) ---\n    ", n, n);
    for (int j = 1; j <= n; j++) printf("%3d", j);
    printf("\n");
    for (int i = 1; i <= n; i++) {
        printf("%2d |", i);
        for (int j = 1; j <= n; j++) {
            printf("%3d", D[i][j]);
        }
        printf("\n");
    }
}

void printDistanceMatrix(int n, int R[MAX_V][MAX_V]) {
    printf("\n--- Матриця відстаней R(D) (%dx%d) ---\n    ", n, n);
    for (int j = 1; j <= n; j++) printf("%3d", j);
    printf("\n");
    for (int i = 1; i <= n; i++) {
        printf("%2d |", i);
        for (int j = 1; j <= n; j++) {
            if (R[i][j] >= INF) {
                printf("  -");
            }
            else {
                printf("%3d", R[i][j]);
            }
        }
        printf("\n");
    }
}

void multiplyBooleanMatrices(int n, int X[MAX_V][MAX_V], int Y[MAX_V][MAX_V], int Res[MAX_V][MAX_V]) {
    int temp[MAX_V][MAX_V] = { 0 };
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            for (int k = 1; k <= n; k++) {
                if (X[i][k] && Y[k][j]) {
                    temp[i][j] = 1;
                    break;
                }
            }
        }
    }
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            Res[i][j] = temp[i][j];
        }
    }
}

void buildReachabilityMatrix(int n, int A[MAX_V][MAX_V], int D[MAX_V][MAX_V]) {
    int Sum[MAX_V][MAX_V] = { 0 };
    int Ak[MAX_V][MAX_V] = { 0 };

    for (int i = 1; i <= n; i++) {
        Sum[i][i] = 1;
        Ak[i][i] = 1;
    }

    for (int step = 1; step <= n - 1; step++) {
        multiplyBooleanMatrices(n, Ak, A, Ak);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (Ak[i][j]) Sum[i][j] = 1;
            }
        }
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            D[i][j] = Sum[i][j];
        }
    }
}

void buildDistanceMatrix(int n, int A[MAX_V][MAX_V], int R[MAX_V][MAX_V]) {
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (i == j) R[i][j] = 0;
            else if (A[i][j] > 0) R[i][j] = 1;
            else R[i][j] = INF;
        }
    }

    for (int d = 2; d <= n - 1; d++) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (R[i][j] == d - 1) {
                    for (int k = 1; k <= n; k++) {
                        if (A[j][k] > 0 && R[i][k] == INF) {
                            R[i][k] = d;
                        }
                    }
                }
            }
        }
    }
}

void analyzeGraph(int type, int n, int D[MAX_V][MAX_V], int R[MAX_V][MAX_V]) {
    char name_buf[32];

    printf("\n--- Аналіз досяжності та транзиту графа G = (V, E), порядок |V| = %d ---\n", n);

    printf("\nШляхи повернення до Офісу (вершина 1):\n");
    int found = 0;
    for (int i = 2; i <= n; i++) {
        getVertexName(i, name_buf, sizeof(name_buf));
        if (D[i][1] == 1) {
            printf("  %s (v%d) -> Офіс: існує (мінімум переміщень = %d)\n", name_buf, i, R[i][1]);
        }
        else {
            printf("  %s (v%d) -> Офіс: шлях відсутній\n", name_buf, i);
            found = 1;
        }
    }
    if (!found) printf("Повернення до Офісу можливе з усіх пунктів.\n");

    int transit[MAX_V] = { 0 };
    int max_transit = 0;

    for (int k = 1; k <= n; k++) {
        for (int u = 1; u <= n; u++) {
            for (int v = 1; v <= n; v++) {
                if (u != v && u != k && v != k) {
                    if (R[u][v] < INF && R[u][k] < INF && R[k][v] < INF) {
                        if (R[u][v] == R[u][k] + R[k][v]) {
                            transit[k]++;
                        }
                    }
                }
            }
        }
        if (transit[k] > max_transit) {
            max_transit = transit[k];
        }
    }

    printf("\nТранзитне навантаження вузлів:\n");
    for (int i = 1; i <= n; i++) {
        getVertexName(i, name_buf, sizeof(name_buf));
        printf("Вершина %d (%-8s): транзитних маршрутів = %d\n", i, name_buf, transit[i]);
    }

    printf("\nКритичні транзитні пункти (max = %d): ", max_transit);
    for (int i = 1; i <= n; i++) {
        if (transit[i] == max_transit) {
            getVertexName(i, name_buf, sizeof(name_buf));
            printf("%s ", name_buf);
        }
    }

    printf("\nПункти, виключені із загального трафіку: ");
    found = 0;
    for (int i = 1; i <= n; i++) {
        int in_paths = 0, out_paths = 0;
        for (int j = 1; j <= n; j++) {
            if (i != j) {
                if (D[j][i] == 1) in_paths++;
                if (D[i][j] == 1) out_paths++;
            }
        }
        if (in_paths == 0 || out_paths == 0) {
            getVertexName(i, name_buf, sizeof(name_buf));
            if (in_paths == 0 && out_paths == 0)
                printf("%s (ізольований) ", name_buf);
            else if (in_paths == 0)
                printf("%s (без входу) ", name_buf);
            else
                printf("%s (тупик / без виходу) ", name_buf);
            found = 1;
        }
    }
    if (!found) printf("немає");

    if (type == 1) {
        int ecc[MAX_V] = { 0 };
        int comp_ecc[MAX_V] = { 0 };
        int diameter = -1;
        int comp_diameter = -1;
        int radius = INF;
        int is_connected = 1;

        for (int i = 1; i <= n; i++) {
            int max_dist = 0;
            int has_unreachable = 0;
            for (int j = 1; j <= n; j++) {
                if (R[i][j] >= INF) {
                    has_unreachable = 1;
                    is_connected = 0;
                }
                else if (R[i][j] > max_dist) {
                    max_dist = R[i][j];
                }
            }
            comp_ecc[i] = max_dist;
            ecc[i] = has_unreachable ? INF : max_dist;

            if (ecc[i] == INF) {
                printf("Вершина %d (%-8s): e(v) = нескінченність (у зв'язній компоненті = %d)\n", i, name_buf, comp_ecc[i]);
            }
            else if (diameter != INF && ecc[i] > diameter) {
                diameter = ecc[i];
            }

            if (comp_ecc[i] > 0) {
                if (comp_ecc[i] > comp_diameter) comp_diameter = comp_ecc[i];
                if (comp_ecc[i] < radius) radius = comp_ecc[i];
            }
        }

        printf("\n\n--- Метричні характеристики графа G = (V, E) ---\n");
        if (!is_connected) {
            printf("Увага: граф незв'язний (наявні ізольовані вузли). Метрики зв'язної частини:\n");
        }

        for (int i = 1; i <= n; i++) {
            getVertexName(i, name_buf, sizeof(name_buf));
            if (ecc[i] == INF) {
                printf("Вершина %d (%-8s): e(v) = нескінченність (ізольований вузол)\n", i, name_buf);
            }
            else {
                printf("Вершина %d (%-8s): e(v) = %d\n", i, name_buf, ecc[i]);
            }
        }

        if (is_connected) {
            printf("\nДіаметр графа D(G) (найгірша затримка): %d", diameter);
        }
        else {
            printf("\nДіаметр графа D(G): нескінченність (діаметр зв'язної компоненти = %d)", comp_diameter);
        }

        if (radius < INF) {
            printf("\nРадіус графа R(G) (найшвидше охоплення): %d", radius);
            printf("\nЦентр графа C(G) (найкраща доступність, e(v) = %d): ", radius);
            for (int i = 1; i <= n; i++) {
                if (comp_ecc[i] == radius) {
                    getVertexName(i, name_buf, sizeof(name_buf));
                    printf("%s ", name_buf);
                }
            }
        }
        else {
            printf("\nРадіус та центр графа: не визначені (усі вершини ізольовані)");
        }
    }

    printf("\n");
}

int main() {
    SetConsoleOutputCP(65001);

    int type, n, m;
    int A[MAX_V][MAX_V] = { 0 };
    int D[MAX_V][MAX_V] = { 0 };
    int R[MAX_V][MAX_V] = { 0 };

    type = getValidInt(
        "Оберіть тип графа (1 - неорієнтований, 2 - орієнтований): ",
        1, 2
    );

    char prompt_n[96], prompt_m[96];

    sprintf_s(prompt_n, sizeof(prompt_n),
        "Введіть порядок графа |V| = n (від 1 до %d): ", MAX_V - 1);

    sprintf_s(prompt_m, sizeof(prompt_m),
        "Введіть кількість ребер |E| = m (від 1 до %d): ", MAX_E - 1);

    n = getValidInt(prompt_n, 1, MAX_V - 1);
    m = getValidInt(prompt_m, 1, MAX_E - 1);

    char prompt_u[64], prompt_v[64];

    sprintf_s(prompt_u, sizeof(prompt_u),
        "  Введіть початкову вершину (1..%d): ", n);

    sprintf_s(prompt_v, sizeof(prompt_v),
        "  Введіть кінцеву вершину (1..%d): ", n);

    for (int e = 1; e <= m; e++) {
        printf("\nРебро e%d:\n", e);

        int u = getValidInt(prompt_u, 1, n);
        int v = getValidInt(prompt_v, 1, n);

        if (type == 1) {
            if (u == v) {
                A[u][u] += 1;
            }
            else {
                A[u][v] += 1;
                A[v][u] += 1;
            }
        }
        else {
            A[u][v] += 1;
        }
    }

    buildReachabilityMatrix(n, A, D);
    buildDistanceMatrix(n, A, R);

    printAdjacencyMatrix(n, A);
    printReachabilityMatrix(n, D);
    printDistanceMatrix(n, R);
    analyzeGraph(type, n, D, R);

    return 0;
}