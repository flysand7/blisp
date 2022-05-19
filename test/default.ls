(do
  (def sum-list (lambda (list)
    (cond (nil? list)
            0
          1
            (int-add (car list) (sum-list (cdr list))))))
  (def sum (lambda list
    (sum-list list)))

  (def vec-scale (lambda ((x y) r)
    (cons (int-mul x r) (cons (int-mul y r) nil))))

  (print (sum 1 2 3))
  (print (vec-scale '(2 -1) 5)))
