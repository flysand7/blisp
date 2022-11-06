

(macro (cond . clauses)
  (if (pair? clauses)
      `(if ,(caar clauses)
           ,(cadr (car clauses))
           (cond ,@(cdr clauses)))))

(def x 1)

(cond
  ((int-eq? x 1) (print 1))
  ((int-eq? x 2) (print 2)))

