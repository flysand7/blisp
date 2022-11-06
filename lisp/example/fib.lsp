
(def (fib n)
  (def (even? n)
    (= (% n 2) 0))
  (def (sq n)
    (* n n))
  (def (fib-iter a b p q count)
    (cond
      ((= count 0)
        b)
      ((even? count)
        (fib-iter a b (+ (sq p) (sq q)) (+ (* 2 p q) (sq q)) (// count 2)))
      (else
        (fib-iter (+ (* b q) (* a q) (* a p)) (+ (* b p) (* a q)) p q (dec count)))))
  (fib-iter 1 0 0 1 n))

(print (map (range 0 30) (\ (n) (fib n))))

