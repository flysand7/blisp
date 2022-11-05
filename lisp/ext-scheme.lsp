
(print "[blisp] Scheme extension loaded")

(def (caar x) (car (car x)))
(def (cadr x) (car (cdr x)))

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

(def (is-sym-eq? thing other)
  (if (sym? thing)
    (if (sym-eq? thing other)
      1
      0)
    0))

(macro (quasiquote x)
  (if (pair? x)
    (if (is-sym-eq? (car x) 'unquote)
      (cadr x)
      (if (is-sym-eq? (caar x) 'unquote-splicing)
        (list 'append
          (cadr (car x))
          (list 'quasiquote (cdr x)))
        (list 'cons
          (list 'quasiquote (car x))
          (list 'quasiquote (cdr x)))))
    (list 'quote x)))

(macro (binary-and A B) `(if ,A (if ,B 1 0) 0))
(macro (binary-or  A B) `(if ,A 1 (if ,B 1 0)))
(def   (not A) (if A 0 1))

(inc "lisp/math.lsp")
