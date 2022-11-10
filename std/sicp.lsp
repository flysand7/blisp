
(def (abs x)
  (if (< x 0)
    (- x)
    x))

(def (square x)
  (* x x))

(def (average x y)
  (/ (+ x y) 2))

(def (sqrt x)
  (def (sqrt-iter guess x)
    (def (improve-guess)
      (average guess (/ x guess)))
    (def (is-guess-good?)
      (< (abs (- (square guess) x)) 0.000001))
    (if (is-guess-good?)
      guess
      (sqrt-iter (improve-guess) x)))
  (sqrt-iter 1.0 x))

(def (fact n)
  (def (fact-iter product i max-i)
    (if (> i max-i)
      product
      (fact-iter (* i product)
                 (+ i 1)
                 max-i)))
  (fact-iter 1 1 n))

(def (fib n)
  (def (fib-iter a b n)
    (if (= n 0)
      b
      (fib-iter (+ a b) a (- n 1))))
  (fib-iter 1 0 n))

