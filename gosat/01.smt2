(declare-fun a_ack!1 () (_ BitVec 64))
(declare-fun b_ack!2 () (_ BitVec 64))
(assert (fp.eq ((_ to_fp 11 53) a_ack!1) ((_ to_fp 11 53) #x3ff0000000000000)))
(assert (fp.eq ((_ to_fp 11 53) b_ack!2) ((_ to_fp 11 53) #x0000000000000000)))