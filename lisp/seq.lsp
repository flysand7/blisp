(do
  (inc "lisp/math.lsp")

  (def (sin x)
    (taylor-seq x sin-taylor-term))

  (def (cos x)
    (taylor-seq x cos-taylor-term))

  (def (sin-taylor-term n x)
    (def m (+ (* 2 n) 1))
    (/. (* (exp -1 n) (exp x m)) (fact m)))

  (def (cos-taylor-term n x)
    (def m (* 2 n))
    (/. (* (exp -1 n) (exp x m)) (fact m)))

  (def (taylor-seq x taylor-term)
    (Σ 0 (seq-good-order x 0.000001)(\(n) (taylor-term n x))))

  (def (seq-good-enough? x n eps)
    (<= (abs (- (sin-taylor-term n x) (sin-taylor-term (inc n) x)))
        eps))

  (def (seq-good-order x eps)
    (def (helper n)
      (if (seq-good-enough? x n eps)
        n
        (helper (inc n))))
    (helper 0)))
