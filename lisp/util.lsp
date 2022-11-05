
(def (foreach list f)
  (if (nil? list)
    nil
    (if (nil? (cdr list))
      (f (car list))
      (do
        (f (car list))
        (foreach (cdr list) f)))))

(def (map list f)
  (if (nil? list)
    nil
    (cons (f (car list)) (map (cdr list) f))))

(def (reduce list f i)
  (if (nil? list)
    i
    (f (car list) (reduce (cdr list) f i))))

(def (reverse list)
  (def (rev-into list rev)
    (if (nil? list)
      rev
      (rev-into (cdr list)
                (cons (car list)
                      rev))))
  (rev-into list nil))

(def (range lo hi)
  (if (int-eq? lo hi)
    (cons lo nil)
    (cons lo (range (int-add 1 lo) hi))))

(def (len list)
  (if (nil? list)
    0
    (int-add 1 (len (cdr list)))))
