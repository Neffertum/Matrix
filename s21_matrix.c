#include "s21_matrix.h"

int s21_create_matrix(int rows, int columns, matrix_t *result) {
  int flag = OK;
  if (result == NULL) {
    flag = INCORRECT_MATRIX;
  } else {
    result->rows = rows;
    result->columns = columns;
    if (result->rows > 0 && result->columns > 0) {
      result->matrix = calloc(result->rows, sizeof(double *));
      if (result->matrix == NULL) {
        s21_remove_matrix(result);
        flag = INCORRECT_MATRIX;
      } else {
        for (int i = 0; i < result->rows; i++) {
          result->matrix[i] = calloc(result->columns, sizeof(double));
          if (result->matrix[i] == NULL) {
            s21_remove_matrix(result);
            flag = INCORRECT_MATRIX;
            break;
          }
        }
      }
    } else {
      result->matrix = NULL;
      flag = INCORRECT_MATRIX;
    }
  }
  return flag;
}

int s21_check_matrix(matrix_t *A) {
  int flag = OK;
  if (A == NULL || A->matrix == NULL || A->rows < 1 || A->columns < 1) {
    flag = INCORRECT_MATRIX;
  }
  return flag;
}

void s21_remove_matrix(matrix_t *A) {
  if (A != NULL && A->matrix != NULL) {
    for (int i = 0; i < A->rows; i++) {
      free(A->matrix[i]);
    }
    free(A->matrix);
    A->rows = 0;
    A->columns = 0;
    A = NULL;
  }
}

int s21_eq_matrix(matrix_t *A, matrix_t *B) {
  int flag = SUCCESS;
  if (A == NULL || B == NULL || A->matrix == NULL || B->matrix == NULL) {
    flag = FAILURE;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    flag = FAILURE;
  } else {
    for (int i = 0; i < A->rows; i++) {
      for (int j = 0; j < A->columns; j++) {
        if (fabs(A->matrix[i][j] - B->matrix[i][j]) > 1e-7) {
          flag = FAILURE;
          break;
        }
      }
      if (flag == FAILURE) {
        break;
      }
    }
  }
  return flag;
}

int s21_sum_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int flag = OK;
  if (s21_check_matrix(A) != OK || s21_check_matrix(B) != OK) {
    flag = INCORRECT_MATRIX;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    flag = CALC_ERROR;
  } else {
    flag = s21_create_matrix(A->rows, A->columns, result);
    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] + B->matrix[i][j];
        }
      }
    }
  }
  return flag;
}

int s21_sub_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int flag = OK;
  if (s21_check_matrix(A) != OK || s21_check_matrix(B) != OK) {
    flag = INCORRECT_MATRIX;
  } else if (A->rows != B->rows || A->columns != B->columns) {
    flag = CALC_ERROR;
  } else {
    flag = s21_create_matrix(A->rows, A->columns, result);
    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] - B->matrix[i][j];
        }
      }
    }
  }
  return flag;
}

int s21_mult_matrix(matrix_t *A, matrix_t *B, matrix_t *result) {
  int flag = OK;
  if (s21_check_matrix(A) != OK || s21_check_matrix(B) != OK) {
    flag = INCORRECT_MATRIX;
  } else if (A->columns != B->rows) {
    flag = CALC_ERROR;
  } else {
    flag = s21_create_matrix(A->rows, B->columns, result);
    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < B->columns; j++) {
          for (int t = 0; t < B->rows; t++) {
            result->matrix[i][j] += A->matrix[i][t] * B->matrix[t][j];
          }
        }
      }
    }
  }
  return flag;
}

int s21_mult_number(matrix_t *A, double number, matrix_t *result) {
  int flag = OK;
  if (s21_check_matrix(A) == OK) {
    flag = s21_create_matrix(A->rows, A->columns, result);
    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[i][j] = A->matrix[i][j] * number;
        }
      }
    }
  } else {
    flag = INCORRECT_MATRIX;
  }
  return flag;
}

int s21_transpose(matrix_t *A, matrix_t *result) {
  int flag = s21_check_matrix(A);
  if (flag == OK) {
    flag = s21_create_matrix(A->columns, A->rows, result);
    if (flag == OK) {
      for (int i = 0; i < A->rows; i++) {
        for (int j = 0; j < A->columns; j++) {
          result->matrix[j][i] = A->matrix[i][j];
        }
      }
    }
  }
  return flag;
}

int s21_determinant(matrix_t *A, double *result) {
  *result = 0;
  int flag = s21_check_matrix(A);
  if (flag == OK) {
    if (A->rows != A->columns) {
      flag = CALC_ERROR;
    } else {
      if (A->rows == 1) {
        *result = A->matrix[0][0];
      } else if (A->rows == 2) {
        *result = A->matrix[0][0] * A->matrix[1][1] -
                  A->matrix[0][1] * A->matrix[1][0];
      } else {
        for (int j = 0; j < A->columns; j++) {
          matrix_t buffer;
          double temp = 0;
          s21_create_matrix(A->rows - 1, A->columns - 1, &buffer);
          if (s21_check_matrix(&buffer) == OK) {
            s21_minor(A, 0, j, &buffer);
            s21_determinant(&buffer, &temp);
            *result += A->matrix[0][j] * pow(-1, (1 + (j + 1))) * temp;
            s21_remove_matrix(&buffer);
          } else {
            flag = INCORRECT_MATRIX;
            break;
          }
        }
      }
    }
  }
  return flag;
}

void s21_minor(matrix_t *A, int l, int k, matrix_t *buffer) {
  int r = 0;
  for (int i = 0; i < A->rows; i++) {
    if (l != i) {
      int t = 0;
      for (int j = 0; j < A->columns; j++) {
        if (k != j) {
          buffer->matrix[r][t] = A->matrix[i][j];
          t++;
        }
      }
      r++;
    }
  }
}

int s21_calc_complements(matrix_t *A, matrix_t *result) {
  int flag = s21_check_matrix(A);
  if (flag == OK) {
    flag = s21_create_matrix(A->rows, A->columns, result);
    if (flag == OK) {
      if (A->rows != A->columns || A->rows < 2) {
        flag = CALC_ERROR;
      } else {
        for (int i = 0; i < A->rows; i++) {
          for (int j = 0; j < A->columns; j++) {
            double determinant = 0;
            matrix_t minor;
            flag = s21_create_matrix(A->rows - 1, A->columns - 1, &minor);
            if (flag == OK) {
              s21_minor(A, i, j, &minor);
              s21_determinant(&minor, &determinant);
              result->matrix[i][j] = pow(-1, i + j) * determinant;
              s21_remove_matrix(&minor);
            } else {
              break;
            }
          }
        }
      }
    }
  }
  return flag;
}

int s21_inverse_matrix(matrix_t *A, matrix_t *result) {
  int flag = s21_check_matrix(A);
  if (flag == OK) {
    if (A->rows == A->columns) {
      double determinant = 0;
      s21_determinant(A, &determinant);
      if (fabs(determinant) > 1e-7) {
        matrix_t buffer;
        matrix_t buffer2;
        if (s21_calc_complements(A, &buffer) == OK &&
            s21_transpose(&buffer, &buffer2) == OK &&
            s21_mult_number(&buffer2, 1 / determinant, result) == OK) {
          s21_remove_matrix(&buffer);
          s21_remove_matrix(&buffer2);
        } else {
          flag = INCORRECT_MATRIX;
        }
      } else {
        flag = CALC_ERROR;
      }
    } else {
      flag = CALC_ERROR;
    }
  } else {
    flag = INCORRECT_MATRIX;
  }

  return flag;
}