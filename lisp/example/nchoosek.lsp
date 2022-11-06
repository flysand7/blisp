
(def (nchoosek n k)
  (cond
    ((= k 0) 1)
    ((= k n) 1)
    (else
      (+
        (nchoosek (dec n) (dec k))
        (nchoosek (dec n) k)))))

(def (print-triag rows)
  (foreach (range 0 rows) (\ (n)
    (print (map (range 0 n) (\ (k) (nchoosek n k)))))))

(print-triag 10)