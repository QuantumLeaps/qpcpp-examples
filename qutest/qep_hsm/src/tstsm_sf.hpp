#ifndef TSTSM_SF_HPP_
#define TSTSM_SF_HPP_

namespace APP {

extern QP::QAsm * const the_TstSM0;
extern QP::QAsm * const the_TstSM1;
extern QP::QAsm * const the_TstSM2;
extern QP::QAsm * const the_TstSM3;
extern QP::QAsm * const the_TstSM4;
extern QP::QAsm * const the_TstSM5;
extern QP::QAsm * const the_TstSM6;

bool TstSM6_isIn(std::uint32_t const state_num);
QP::QStateHandler TstSM6_child(std::uint32_t const  state_num);

} // namespace APP


#endif // TSTSM_SF_HPP_
