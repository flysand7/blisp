(do
  (def (int-sqr x) (int-mul x x))

  (def (sum . list)
    (cond
      (nil? list)
        0
      1
        (int-add (car list) (apply sum nil))))
  (print (sum 3 4 -1)))
