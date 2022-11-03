
(def (count n a)
  (if (int-eq? n 0)
      a
      (count (int-sub n 1) (int-add a 1))))

(count 100000 0)
