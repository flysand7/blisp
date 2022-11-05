
(def (sqrt-iter prev-guess guess precision a)
  (def (good-enough?)
    (< (abs (- prev-guess guess)) precision))
  (def (improved-guess)
    (average guess (/ a guess)))
  (if (good-enough?)
    guess
    (sqrt-iter guess (improved-guess) precision a)))

(def (cbrt-iter prev-guess guess precision a)
  (def (good-enough?)
    (< (abs (- prev-guess guess)) precision))
  (def (improved-guess)
    (/ (+ (/ a (square guess)) (* 2 guess)) 3))
  (if (good-enough?)
    guess
    (cbrt-iter guess (improved-guess) precision a)))


(def (sqrt a)
  (sqrt-iter INFINITY 1.0 0.0000001 a))

(def (cbrt a)
  (cbrt-iter INFINITY 1.0 0.0000001 a))

(print (cbrt 0))
(print (cbrt 1))
(print (cbrt 2))
(print (cbrt 3))
(print (cbrt 8))
