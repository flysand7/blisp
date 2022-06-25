(inc "lisp/math.lsp")

(def (count n)
  (if (= n 0)
    1
    (count (- n 1))))

(print (count 100))
