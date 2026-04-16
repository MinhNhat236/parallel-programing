#ifndef MATRIX_H
#define MATRIX_H
#include <vector>
#include <ostream>

template <typename T>
class Matrix {
public:
    Matrix() : rows_(), columns_() {}
    Matrix(size_t n) : rows_(n), columns_(n) { this->allocate_(); }
    Matrix(size_t n, size_t m) : rows_(n), columns_(m) { this->allocate_(); }
    Matrix(size_t n, size_t m, const std::vector<T>& arr) : rows_(n), columns_(m) {
        this->allocate_();
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                data_[i * columns_ + j] = arr[i * columns_ + j];
            }
        }
    }

    size_t rows() const { return rows_; }
    size_t columns() const { return columns_; }

    Matrix(const Matrix& m) {
        this->rows_ = m.rows_;
        this->columns_ = m.columns_;
        this->allocate_();
        this->copy_data_(m);
    }

    Matrix<T>& operator=(const Matrix<T>& m) {
        if (this != &m) {
            if (this->rows_ != m.rows_ || this->columns_ != m.columns_) {
                this->destroy_();
                this->rows_ = m.rows_;
                this->columns_ = m.columns_;
                this->allocate_();
            }
            copy_data_(m);
        }
        return *this;
    }

    ~Matrix() {
        this->destroy_();
    }

    T& operator()(size_t n, size_t m) {
        if (n >= rows_ || m >= columns_) {
            throw "Выход за границы матрицы!";
        }
        return this->data_[n * columns_ + m];
    }
    T operator()(size_t n, size_t m) const {
        if (n >= rows_ || m >= columns_) {
            throw "Выход за границы матрицы!";
        }
        return this->data_[n * columns_ + m];
    }

    bool operator==(const Matrix<T>& other) const {
        if (rows_ != rows_ || columns_ != columns_) {
            return false;
        }
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                if (data_[i * columns_ + j] != other.data_[i * columns_ + j]) {
                    return false;
                }
            }
        }
        return true;
    }

    Matrix<T>& operator+=(const Matrix& other) {
        if (rows_ != other.rows_ || columns_ != other.columns_) {
            throw "Матрицы разных размеров";
        }
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                data_[i * columns_ + j] += other.data_[i * columns_ + j];
            }
        }
        return *this;
    }

    Matrix<T>& operator-=(const Matrix& other) {
        if (rows_ != other.rows_ || columns_ != other.columns_) {
            throw "Матрицы разных размеров";
        }
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                data_[i * columns_ + j] -= other.data_[i * columns_ + j];
            }
        }
        return *this;
    }

    Matrix<T>& operator*=(const Matrix<T>& other) {
        if (columns_ != other.rows_) {
            throw "Матрицы не совместны";
        }
        Matrix<T> res(rows_, other.columns_);

        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < other.columns_; j++) {
                for (size_t r = 0; r < columns_; r++) {
                    res.data_[i * other.columns_ + j] += data_[i * columns_ + r] * other.data_[r * other.columns_ + j];
                }
            }
        }
        *this = res;
        return *this;
    }

    Matrix<T>& operator*=(T scalar) {
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                data_[i * columns_ + j] *= scalar;
            }
        }
        return *this;
    }

    Matrix<T>& operator/=(T scalar) {
        if (scalar == T(0)) {
            throw "Деление на 0!";
        }
        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                data_[i * columns_ + j] /= scalar;
            }
        }
        return *this;
    }

    void transpose() {
        Matrix<T> res(columns_, rows_);

        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                res(j, i) = data_[i * columns_ + j];
                std::cout << data_[i * columns_ + j] << "\n";
            }
        }
        *this = res;
    }

    Matrix<T> get_transposed() const {
        Matrix<T> res(columns_, rows_);

        for (size_t i = 0; i < rows_; i++) {
            for (size_t j = 0; j < columns_; j++) {
                res(j, i) = data_[i * columns_ + j];
                std::cout << data_[i * columns_ + j] << "\n";
            }
        }
        return res;
    }

    /* Похожий функционал, как у std::vector */
    T* data() {
        return data_;
    }

private:
    void allocate_() {
        this->data_ = new T[rows_ * columns_]{ 0 };
    }
    void destroy_() {
        delete[] data_;
    }
    void copy_data_(const Matrix& m) {
        for (size_t i = 0; i < m.rows_; i++) {
            for (size_t j = 0; j < m.columns_; j++) {
                this->data_[i * columns_ + j] = m.data_[i * columns_ + j];
            }
        }
    }

    T* data_ = 0;
    size_t rows_;
    size_t columns_;
};

template <typename T>
bool operator==(const Matrix<T>& a, const Matrix<T>& b) {
    return a == (b);
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m, T scalar) {
    return Matrix<T>(m) *= scalar;
}

template <typename T>
Matrix<T> operator*(T scalar, const Matrix<T>& m) {
    return Matrix<T>(m) *= scalar;
}

template <typename T>
Matrix<T> operator/(const Matrix<T>& m, T scalar) {
    return Matrix<T>(m) /= scalar;
}

template <typename T>
Matrix<T> operator+(const Matrix<T>& m1, const Matrix<T>& m2) {
    return Matrix<T>(m1) += m2;
}

template <typename T>
Matrix<T> operator-(const Matrix<T>& m1, const Matrix<T>& m2) {
    return Matrix<T>(m1) += m2;
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& m1, const Matrix<T>& m2) {
    return Matrix<T>(m1) *= m2;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m) {
    for (size_t i = 0; i < m.rows(); i++) {
        for (size_t j = 0; j < m.columns(); j++) {
            os << m(i, j) << " ";
        }
        os << "\n";
    }
    return os;
}

#endif // MATRIX_H