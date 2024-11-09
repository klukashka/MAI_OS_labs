// how to compile g++ -pthread main.cpp
// ./a.out 20 10000 10000 1 & top -H -p $! -d 0.1

#include <iostream>
#include <pthread.h>
#include <random>
#include <vector>
#include <time.h>

std::mt19937 rng(time(0));

// pthread_mutex_t mutex;

const int e_f_size = 5;
const int erosion_filter[e_f_size][e_f_size] = {
    {0, 0, 1, 0, 0}, 
    {0, 1, 1, 1, 0}, 
    {1, 1, 1, 1, 1}, 
    {0, 1, 1, 1, 0}, 
    {0, 0, 1, 0, 0}, 
};

const int j_f_size = 5;
const int joint_filter[j_f_size][j_f_size] = {
    {0, 0, -1, 0, 0}, 
    {0, -1, -1, -1, 0}, 
    {-1, -1, -1, -1, -1}, 
    {0, -1, -1, -1, 0}, 
    {0, 0, -1, 0, 0}, 
};

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


void make_rand_matrix(const int n, const int m, std::vector<std::vector<int>>& matrix){
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j){
            matrix[i][j] = rng() % 256;
        }
    }
}

void print_mat(const int n, const int m, std::vector<std::vector<int>>& mat){
    for (int i = 0; i < n; ++i){
        for (int j = 0; j < m; ++j){
            std::cout << mat[i][j] << ' ';
        }
        std::cout << '\n';
    }
}

struct thrargs{
    int n, m, num_threads;
    int ind;
    std::vector<std::vector<int>> *mat_ori1, *mat_ori2, *mat_cop1, *mat_cop2;
};

void do_filter(
    char t, 
    const int i_ind, 
    const int j_ind, 
    const int n,
    const int m, 
    std::vector<std::vector<int>> *mat_ori, 
    std::vector<std::vector<int>> *mat_cop
    ){
    int res = 0;
    const int e_shift = e_f_size/2;
    const int j_shift = j_f_size/2;
    if (t == 'e'){
        for (int i = -e_shift; i <= e_shift; ++i){
            for (int j = -e_shift; j <= e_shift; ++j){
                // std::cout << i << ' ' << j << ' ' << i_ind << ' '  << j_ind << '\n';
                // std::cout << i+e_shift << ' ' << j+e_shift << '\n';
                if (i_ind + i >= 0 && j_ind + j >= 0 && i_ind + i < n && j_ind + j < m){
                    res += erosion_filter[i+e_shift][j+e_shift] * (*mat_ori)[i_ind+i][j_ind+j]/10;
                }
            }
        }
    } else {
        for (int i = -j_shift; i <= j_shift; ++i){
            for (int j = -j_shift; j <= j_shift; ++j){
                if (i_ind + i >= 0 && j_ind + j >= 0 && i_ind + i < n && j_ind + j < m){
                    res += joint_filter[i+j_shift][j+j_shift] * (*mat_ori)[i_ind+i][j_ind+j]/10;
                }
            }
        }
    }
    // std::cout << res << " res\n";
    (*mat_cop)[i_ind][j_ind] = std::min(255, std::max(res, 0));
    // std::cout << mat_cop[i_ind][j_ind] << '\n';
}

void* use_filters(void* thr){
    const int n = ((thrargs*)thr)->n;
    const int m = ((thrargs*)thr)->m;
    const int num_threads = ((thrargs*)thr)->num_threads;
    const int ind = ((thrargs*)thr)->ind;
    const int total = n * m;

    const int part = total / num_threads;

    int first = ind * part;
    int last = (first + 2*part > total ? total : first + part);

    int i = first / m;
    int j = first % m;
    int row_last = last / m;
    int col_last = last % m;
    // pthread_mutex_lock(&mutex);
    // std::cout << ind << " hgdsgf\n";
    // pthread_mutex_unlock(&mutex);
    do {
        do_filter('e', i, j, n, m, ((thrargs*)thr)->mat_ori1, ((thrargs*)thr)->mat_cop1);
        do_filter('j', i, j, n, m, ((thrargs*)thr)->mat_ori2, ((thrargs*)thr)->mat_cop2);

        if (j + 1 == m){
            j = 0;
            ++i;
        } else {
            ++j;
        }
    } while (!(i == row_last && j == col_last));
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char* argv[]) {
    // maybe it is better to make filters and matrixes with doubles though there's no need
    // clock_t begin = clock();
    struct timespec start, finish;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (argc != 5){
        std::cerr << "Wrong number of arguments\n";
        std::cerr << "Usage: number_of_treads n m k" << std::endl;
        return 1;
    }
    if (!(is_number(argv[1]) && is_number(argv[2])) && is_number(argv[3]) && is_number(argv[4])){
        std::cerr << "Wrong type of arguments" << std::endl;
        return 1;
    }

    const int n = atoi(argv[2]);
    const int m = atoi(argv[3]);
    const int k = atoi(argv[4]);

    const int num_threads = std::min(atoi(argv[1]), n*m);
    std::cout << num_threads << '\n';

    if (num_threads <= 0){
        std::cerr << "Wrong argument: number of threads should be positive" << std::endl;
        return 1;
    }
    if (std::max(e_f_size, j_f_size) > std::min(n, m)){
        std::cerr << "Wrong argument: inappropriate size ratio" << std::endl;
        return 1;
    }
    if (k <= 0){
        std::cerr << "Wrong argument: k value should be positive" << std::endl;
        return 1;
    }
    
    std::vector<std::vector<int>> mat_ori1(n, std::vector<int>(m));
    std::vector<std::vector<int>> mat_ori2(n, std::vector<int>(m));
    std::vector<std::vector<int>> mat_cop1(n, std::vector<int>(m));
    std::vector<std::vector<int>> mat_cop2(n, std::vector<int>(m));

    make_rand_matrix(n, m, mat_ori1);
    std::cout << "ORIGINAL MATRIX:\n";
    // print_mat(n, m, mat_ori1);

    copy(mat_ori1.begin(), mat_ori1.end(), mat_ori2.begin());
    // copy(mat_ori1.begin(), mat_ori1.end(), mat_cop1.begin());
    // copy(mat_ori1.begin(), mat_ori1.end(), mat_cop2.begin());

    // pthread_mutex_init(&mutex, NULL);
    pthread_t tid[num_threads];
    std::vector<thrargs> vec_thr(num_threads);

    for (int k_iter = 0; k_iter < k; ++k_iter){
        for (int i = 0; i < num_threads; ++i){
            thrargs thr = {n, m, num_threads, i, &mat_ori1, &mat_ori2, &mat_cop1, &mat_cop2};
            vec_thr[i] = thr;
            if (pthread_create(&tid[i], NULL, use_filters, &vec_thr[i]) != 0){
                std::cerr << "Error: failed to create a thread" << std::endl;
                return 1;
            }
            // use_filters(&thr);
        }

        for (int i = 0; i < num_threads; ++i){
            if (pthread_join(tid[i], NULL) != 0){
                std::cerr << "Error: failed to join a thread" << std::endl;
                return 1;
            }
        }
        mat_ori1 = mat_cop1;
        mat_ori2 = mat_cop2;
    }

    std::cout << "MATRIX 1:\n";
    // print_mat(n, m, mat_cop1);
    // std::cout << "MATRIX 2:\n";
    // print_mat(n, m, mat_cop2);

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    std::cout << "Execution time: " << elapsed << '\n';

    return 0;
}