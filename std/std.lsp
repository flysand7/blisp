
(print "[blisp] standard extension loaded")

; Generic error function
(def (error . str)
  (print "[error]:" str))

; Returns the type name of a variable
(def (type-name x)
  (if (nil? x) "NIL"
  (if (int? x) "INT"
  (if (flt? x) "FLT"
  (if (sym? x) "SYM"
  (if (func? x) "FUNC"
  (if (pair? x) "PAIR"
    "UNKNOWN")))))))

; List manipulation utilities

(def (foldl f init list)
  (if (nil? list)
    init
    (foldl
      f
      (f init (car list))
      (cdr list))))

(def (foldr f init list)
  (if (nil? list)
    init
    (f (car list) (foldr f init (cdr list)))))

(def (map-unary f list)
  (foldr (\ (x rest) (cons (f x) rest))
    nil
    list))

(def (map f . arg-lists)
  (if (nil? (car arg-lists))
    nil
    (cons (apply f (map-unary car arg-lists))
          (apply map  (cons f (map-unary cdr arg-lists))))))

(def (list . items)
  (foldr cons nil items))

(def (reverse list)
  (foldl (\ (a x) (cons x a)) nil list))

(def (append a b)
  (foldr cons b a))

; Determines whether the two objects are equal regardless of type

(def (eq? a b)
  (if (nil? a) (nil? b)
  (if (int? a) (if (int? b) (int-eq? a b) 0)
  (if (flt? a) (if (flt? b) (flt-eq? a b) 0)
  (if (sym? a) (if (sym? b) (sym-eq? a b) 0)
  (if (func? a) (error "[eq]: Can't compare functions")
  (if (func? b) (error "[eq]: Can't compare functions")
  (if (pair? a)
    (if (pair? b)
      (if (eq? (car a) (car b))
        (eq? (cdr a) (cdr b))
        0)
      0)
    0))))))))

; Quasiquotation

(def (unquote? x)
  (eq? (car x) 'unquote))

(def (unquote-splicing? x)
  (if (pair? (car x))
    (eq? (car (car x)) 'unquote-splicing)
    0))

(macro (quasiquote x)
  (if (pair? x)
      (if (unquote? x)
          (car (cdr x))
          (if (unquote-splicing? x)
              (list 'append
                    (car (cdr (car x)))
                    (list 'quasiquote (cdr x)))
              (list 'cons
                    (list 'quasiquote (car x))
                    (list 'quasiquote (cdr x)))))
      (list 'quote x)))

; Utility macros

(macro (cond . clauses)
  (if (pair? clauses)
      `(if ,(car (car clauses))
           ,(car (cdr (car clauses)))
           (cond ,@(cdr clauses)))
        (error "clauses isnt a pair")))

(def else 1)

(macro (do . stmts)
  `((\ () ,@stmts)))

; Boolean

(def   (not A) (if A 0 1))

(macro (and . args)
  (if (nil? (cdr args))
    (car args)
    `(if ,(car args) (if (and ,@(cdr args)) 1 0) 0)))

(macro (or . args)
  (if (nil? (cdr args))
    (car args)
    `(if ,(car args) 1 (if (or ,@(cdr args)) 1 0))))

; Math

(def (to-flt x)
  (if (flt? x) x
  (if (int? x) (flt-from-int x)
               (error "Can't convert type" (type-name x) "to float"))))

(def (homo-bin-op int-f flt-f x y)
  (cond
      ((int? x) (cond ((int? y) (int-f x y))
                      ((flt? y) (flt-f (flt-from-int x) y))
                      (else     'type_error)))
      ((flt? x) (cond ((int? y) (flt-f x (flt-from-int y)))
                      ((flt? y) (flt-f x y))
                      (else     'type_error)))
      (else
        'type_error)))

(def (homo-bin-add x y)
  (homo-bin-op int-add flt-add x y))
(def (homo-bin-sub x y)
  (homo-bin-op int-sub flt-sub x y))
(def (homo-bin-mul x y)
  (homo-bin-op int-mul flt-mul x y))
(def (homo-bin-div x y)
  (homo-bin-op int-div flt-div x y))

(def (+ . list)
  (foldl (\ (a b) (homo-bin-add a b)) 0 list))

(def (* . list)
  (foldl (\ (a b) (homo-bin-mul a b)) 1.0 list))

(def (/ a b)
  (if (eq? b 0)
    (error "Division by zero")
    (flt-div (to-flt a) (to-flt b))))

(def (// a b)
  (if (eq? b 0)
    (error "Division by zero")
    (int-div a b)))

(def (% a b)
  (if (eq? b 0)
    a
    (int-rem a b)))

(def (- . list)
  (if (eq? (cdr list) nil)
    (flt-neg (to-flt (car list)))
    (if (eq? (cdr (cdr list)) nil)
      (homo-bin-sub (car list) (car (cdr list)))
      (error "Subtraction can't take more than 2 args"))))

; Comparison

(def (< a b)
  (homo-bin-op int-les? flt-les? a b))

(def (> a b)
  (homo-bin-op int-grt? flt-grt? a b))

(def (= a b)
  (homo-bin-op int-eq? flt-eq? a b))

(def (!= a b)
  (homo-bin-op int-neq? flt-neq? a b))

(def (<= a b)
  (homo-bin-op int-leq? flt-leq? a b))

(def (>= a b)
  (homo-bin-op int-geq? flt-geq? a b))
