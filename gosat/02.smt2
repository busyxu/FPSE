(declare-fun FPCHECK_FADD_UNDERFLOW ((_ BitVec 64) (_ BitVec 64)) Bool)
(declare-fun b_ack!3 () (_ BitVec 64))
(declare-fun a_ack!2 () (_ BitVec 64))
(assert (FPCHECK_FADD_UNDERFLOW a_ack!2 b_ack!3))