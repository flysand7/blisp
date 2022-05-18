(do
  (def * int-mul)
  (def exp (lambda (x n)
    (cond (int-eq n 0) 1
          1            (int-mul x (exp x (int-sub n 1))))))
  (def fact (lambda (n)
    (cond (int-eq n 0) 1
          1            (int-mul n (fact (int-sub n 1))))))
  (def choose (lambda (n k)
    (int-div (fact n) (int-mul (fact k) (fact (int-sub n k))))))
  (print (* 2 3)))
