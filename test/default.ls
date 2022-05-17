(do
  (include env 'test/ops.ls)
  (include env 'test/bool.ls)
  (include env 'test/infix.ls)

  (define mylist '(0 1 2 3 4 5))
  (print (mapn mylist (lambda (n el) (* n el)))) ; -> (0 1 4 9 16 25)
  ;(print (infix 2))
  ;(print (infix 2 + -3)))
  