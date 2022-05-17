(do

  (include env 'test/bool.ls)

  (define listn (lambda (list)
    (if (nil? list) 0
                    (+ 1 (listn (cdr list))))))

  (define for (lambda (start iterations F)
    (define for-helper (lambda (s i F)
      (if (< s i) (do (F s)
                      (for-helper (+ s 1) i F))
                   nil)))
    (for-helper start iterations F)))

  (define map (lambda (f list)
    (if (nil? list) nil
                    (cons (f (car list))
                          (map f (cdr list))))))

  (define mapn (lambda (list f)
    (define mapn-helper (lambda (n f list)
      (if (nil? list) nil
                      (cons (f n (car list))
                            (mapn-helper (+ n 1) f (cdr list))))))
    (mapn-helper 0 f list))))