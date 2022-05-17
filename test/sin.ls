((lambda ()
  (define fact (lambda (n)
    (if (== n 0)
      1
      (* n (fact (- n 1))))))

  (define exp (lambda (x n)
    (if (== n 0)
      1
      (* x (exp x (- n 1))))))

  (define abs (lambda (x)
    (if (< x 0)
        (- 0 x)
        x)))

  (define sin-term (lambda (n)
    (lambda (x)
      (/ (* (exp -1 n)
            (exp x (+ (* 2 n) 1)))
         (fact (+ (* 2 n) 1))))))

  (define eps 0.0001)

  (define alt-seq-term-cond (lambda (n x term)
    (< (abs (- ((term n) x)
               ((term (+ n 1) x))))
       eps)))

  (define alt-sum (lambda (n terminate? term)
    (lambda (x)
      (if (terminate? n x term)
        (term n)
        (+ (term n) (alt-sum (+ n 1) terminate? term))))))

  (define sin (sum 0 alt-seq-term-cond sin-term))))