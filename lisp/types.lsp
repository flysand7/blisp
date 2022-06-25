
(def (type? expr)
  (if (and (pair? expr)
      (and (sym? expr)
           (sym-eq? expr 'type)))))

(def (type expr)
  (cond ((int? expr)  '(type int))
        ((flt? expr)  '(type flt))
        ((str? expr)  '(type str))
        ((sym? expr)  '(type sym))
        ((pair? expr) '(type pair))
        ((nil? expr)  '(type nil))
        ((type? expr) '(type type))))

(def (type-eq? expr1 expr2)
  (cond ((not (type? expr1)) (type-eq? (type expr1) expr2))
        ((not (type? expr2)) (type-eq? expr1 (type expr2)))
        (sym-eq (car (cdr expr1)) (car (cdr (expr2))))))

(print (type 'int))
(print (type 2))