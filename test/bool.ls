(do
  (define if (lambda (ok? 'branch-true 'branch-false)
    (cond
      ok? (eval env branch-true)
      1   (eval env branch-false))))

  (define not (lambda (x)    (- 1 x)))
  (define or  (lambda (x 'y) (if x       1 (eval env y))))
  (define and (lambda (x 'y) (if (not x) 0 (eval env y)))))