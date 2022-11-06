
(def (d/dv expr v)
  (cond ((const? expr v)
          0)
        ((same-var? expr v)
          1)
        ((sum? expr)
            (make-sum (d/dv (a1 expr) v)
                      (d/dv (a2 expr) v)))
        ((prod? expr)
          (make-sum (make-prod (m1 expr)
                               (d/dv (m2 expr) v))
                    (make-prod (d/dv (m1 expr) v)
                               (m2 expr))))))
(def (atom? expr)
  (or (nil? expr)
      (or (int? expr)
          (flt? expr))))

(def (const? expr v)
  (or (atom? expr)
      (and (sym? expr) (not (sym-eq? expr v)))))

(def (same-var? expr v)
  (and (sym? expr) (sym-eq? expr v)))

(def (sum? expr)
  (and (list? expr)
       (and (sym? (car expr))
            (sym-eq? (car expr) '+))))

(def (make-sum a1 a2)
  (cond ((and (int? a1) (int-eq? a1 0)) a2)
        ((and (int? a2) (int-eq? a2 0)) a1)
        (1 (list '+ a1 a2))))

(def (a1 expr)
  (car (cdr expr)))

(def (a2 expr)
  (car (cdr (cdr expr))))

(def (prod? expr)
  (and (list? expr)
       (and (sym? (car expr))
            (sym-eq? (car expr) '*))))

(def (make-prod m1 m2)
  (cond ((and (int? m1) (int-eq? m1 1)) m2)
        ((and (int? m2) (int-eq? m2 1)) m1)
        ((and (int? m1) (int-eq? m1 0)) 0)
        ((and (int? m2) (int-eq? m2 0)) 0)
        (1 (list '* m1 m2))))

(def (m1 expr)
  (car (cdr expr)))

(def (m2 expr)
  (car (cdr (cdr expr))))

; f(x) = ax^2 + bx + c
(def f
  '(+ (* a (* x x))
      (+ (* b x)
         c)))

(print "Derivative of" f "is")
(print (d/dv f 'x))