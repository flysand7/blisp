
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

(def (exp x n)
  (def (exp-iter x i product)
    (if (= i 0)
      product
      (exp-iter x (- i 1) (* x product))))
  (exp-iter x n 1))

(def (even? n)
  (int-eq? (int-rem n 2) 0))

(def (fib n)
  (def (fib-iter a b p q i)
    (cond
      ((= i 0)
        b)
      ((even? i)
        (fib-iter
          a
          b
          (+ (square p) (square q))
          (+ (* 2 p q) (square q))
          (int-div i 2)))
      (else
        (fib-iter
          (+ (* b q) (* a q) (* a p))
          (+ (* b p) (* a q))
          p
          q
          (int-sub i 1)))))
  (fib-iter 1 0 0 1 n))

(def (gcd a b)
  (if (= b 0)
    a
    (gcd b (% a b))))

(def (prime? n)
  (def (divides? a b) (eq? (% b a) 0))
  (def (find-divisor n test-divisor)
    (cond
      ((> (square test-divisor) n)
        n)
      ((divides? test-divisor n)
        test-divisor)
      (else
        (find-divisor n (+ test-divisor 1)))))
  (def (smallest-divisor n)
    (find-divisor n 2))
  (= n (smallest-divisor n)))

(def (dec n) (- n 1))
(def (inc n) (+ n 1))

(def (sum f next a b)
  (def (sum-iter f next cur-sum a b)
    (if (> a b)
      cur-sum
      (sum-iter f next (+ cur-sum (f a)) (next a) b)))
  (sum-iter f next 0 a b))

(def (pi-sum a b)
  (def (pi-term x)
    (/ 1.0 (* x (+ x 2))))
  (def (pi-next x)
    (+ x 4))
  (sum pi-term pi-next a b))

(print (pi-sum 1 1000))
