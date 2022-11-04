(inc "lisp/util.lsp")

(def (gen-f int-f flt-f p1 p2)
  (if (int? p1)
    (if (int? p2)
      (int-f p1 p2)
      (if (flt? p2)
        (flt-f (flt-from-int p1) p2)
        'type_error))
    (if (flt? p1)
      (if (int? p2)
        (flt-f p1 (flt-from-int p2))
        (if (flt? p2) (flt-f p1 p2)
          'type_error)))
    'type_error))

(def (gen-add p1 p2)
  (gen-f int-add flt-add p1 p2))

(def (gen-mul p1 p2)
  (gen-f int-mul flt-mul p1 p2))

(def (gen-sub p1 p2)
  (gen-f int-sub flt-sub p1 p2))

(def (gen-div p1 p2)
  (gen-f int-div flt-div p1 p2))

(def (to-flt x)
  (if (flt? x)
    x
    (if (int? x)
      (flt-from-int x)
      'error)))


(def (cflt-div p1 p2)
  (flt-div (to-flt p1) (to-flt p2)))

(def (gen-eq? p1 p2)
  (gen-f int-eq? flt-eq? p1 p2))

(def (gen-neq? p1 p2)
  (gen-f int-neq? flt-neq? p1 p2))

(def (gen-grt? p1 p2)
  (gen-f int-grt? flt-grt? p1 p2))

(def (gen-les? p1 p2)
  (gen-f int-les? flt-les? p1 p2))

(def (gen-geq? p1 p2)
  (gen-f int-geq? flt-geq? p1 p2))

(def (gen-leq? p1 p2)
  (gen-f int-leq? flt-leq? p1 p2))

(def (sum list)
  (reduce list gen-add 0))

(def (mul list)
  (reduce list gen-mul 1))

(def (+ . list)
  (sum list))

(def (* . list)
  (mul list))

(def - gen-sub)
(def / gen-div)
(def /. cflt-div)

(def =  gen-eq?)
(def != gen-neq?)
(def >  gen-grt?)
(def <  gen-les?)
(def >= gen-geq?)
(def <= gen-leq?)

(def (inc x) (gen-add x 1))
(def (dec x) (gen-sub x 1))

; The Σ boi
(def (Σ first last f)
  (sum (map (range first last) f)))

; The Π boi
(def (Π first last f)
  (mul (map (range first last) f)))

(def (fact n)
  (if (= 0 n)
    1
    (Π 1 n (\(n) n))))

(def (exp x n)
  (if (= 0 n)
    1
    (Π 1 n (\(n) x))))

(def (abs n)
  (if (< n 0)
    (- 0 n)
    n))