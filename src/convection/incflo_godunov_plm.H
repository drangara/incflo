#ifndef GODUNOV_PLM_H
#define GODUNOV_PLM_H

#include <incflo_slopes_K.H> 
#include <AMReX_Gpu.H> 
#include <AMReX_FArrayBox.H>
#include <AMReX_BCRec.H> 
#include <AMReX_BC_TYPES.H>
#include <AMReX_Array.H>
#include <iomanip>

namespace {

// This version is called after the MAC projection
AMREX_GPU_DEVICE AMREX_FORCE_INLINE
void Godunov_plm_fpu_x (const int i, const int j, const int k, const int n,
                        const amrex::Real dt, const amrex::Real dx,
                        amrex::Real& Im, amrex::Real& Ip,
                        const amrex::Array4<const amrex::Real> &S,
                        const amrex::Real& umac,
                        const amrex::BCRec bc,
                        const int domain_ilo, const int domain_ihi, bool is_velocity = false)
{
    using namespace amrex;
    {
        bool extdir_or_ho_ilo = (bc.lo(0) == BCType::ext_dir) or
                                (bc.lo(0) == BCType::hoextrap);
        bool extdir_or_ho_ihi = (bc.hi(0) == BCType::ext_dir) or
                                (bc.hi(0) == BCType::hoextrap);

        Real upls, umns;

        if (i == domain_ilo && (bc.lo(0) == BCType::ext_dir)) {
            umns = S(i-1,j,k,n);
            if(n ==0 and is_velocity)
            {
              upls = S(i-1,j,k,n);
            } else {
            upls = S(i  ,j,k,n) + 0.5 * (-1.0 - umac * dt/dx) *
               incflo_ho_xslope_extdir(i  ,j,k,n,S, extdir_or_ho_ilo, extdir_or_ho_ihi, domain_ilo, domain_ihi);
            }
        } else if (i == domain_ihi+1 && (bc.hi(0) == BCType::ext_dir)) {
            upls = S(i  ,j,k,n);
            if(n ==0 and is_velocity)
            {
              umns = S(i,j,k,n);
            } else {
               umns = S(i-1,j,k,n) + 0.5 * ( 1.0 - umac * dt/dx) *
               incflo_ho_xslope_extdir(i-1,j,k,n,S, extdir_or_ho_ilo, extdir_or_ho_ihi, domain_ilo, domain_ihi);
            }
        } else {
            upls = S(i  ,j,k,n) + 0.5 * (-1.0 - umac * dt/dx) *
               incflo_ho_xslope_extdir(i  ,j,k,n,S, extdir_or_ho_ilo, extdir_or_ho_ihi, domain_ilo, domain_ihi);
            umns = S(i-1,j,k,n) + 0.5 * ( 1.0 - umac * dt/dx) *
               incflo_ho_xslope_extdir(i-1,j,k,n,S, extdir_or_ho_ilo, extdir_or_ho_ihi, domain_ilo, domain_ihi);
        }

        Ip = umns;
        Im = upls;
    }
}

// This version is called after the MAC projection
AMREX_GPU_DEVICE AMREX_FORCE_INLINE
void Godunov_plm_fpu_y (const int i, const int j, const int k, const int n,
                        const amrex::Real dt, const amrex::Real dy,
                        amrex::Real& Im, amrex::Real& Ip,
                        const amrex::Array4<const amrex::Real> &S,
                        const amrex::Real& vmac,
                        const amrex::BCRec bc,
                        const int domain_jlo, const int domain_jhi, bool is_velocity = false)
{
    using namespace amrex;
    {
        bool extdir_or_ho_jlo = (bc.lo(1) == BCType::ext_dir) or
                                (bc.lo(1) == BCType::hoextrap);
        bool extdir_or_ho_jhi = (bc.hi(1) == BCType::ext_dir) or
                                (bc.hi(1) == BCType::hoextrap);

        Real vpls, vmns;


        if (j == domain_jlo && (bc.lo(1) == BCType::ext_dir)) {
            vmns = S(i,j-1,k,n);
            if(n==1 and is_velocity)
            {
                vpls = S(i,j-1,k,n);
            } else {
              vpls = S(i,j  ,k,n) + 0.5 * (-1.0 - vmac * dt/dy) *
                 incflo_ho_yslope_extdir(i,j  ,k,n,S, extdir_or_ho_jlo, extdir_or_ho_jhi, domain_jlo, domain_jhi);
            }
        } else if (j == domain_jhi+1 && (bc.hi(1) == BCType::ext_dir)) {
            vpls = S(i,j  ,k,n);
            if(n==1 and is_velocity)
            {
                vmns = S(i,j  ,k,n);
            } else {
            vmns = S(i,j-1,k,n) + 0.5 * ( 1.0 - vmac * dt/dy) *
               incflo_ho_yslope_extdir(i,j-1,k,n,S, extdir_or_ho_jlo, extdir_or_ho_jhi, domain_jlo, domain_jhi);
            }

        } else {
            vpls = S(i,j  ,k,n) + 0.5 * (-1.0 - vmac * dt/dy) *
               incflo_ho_yslope_extdir(i,j  ,k,n,S, extdir_or_ho_jlo, extdir_or_ho_jhi, domain_jlo, domain_jhi);
            vmns = S(i,j-1,k,n) + 0.5 * ( 1.0 - vmac * dt/dy) *
               incflo_ho_yslope_extdir(i,j-1,k,n,S, extdir_or_ho_jlo, extdir_or_ho_jhi, domain_jlo, domain_jhi);
        }


        Ip = vmns;
        Im = vpls;
    }
}

#if (AMREX_SPACEDIM == 3)
// This version is called after the MAC projection
AMREX_GPU_DEVICE AMREX_FORCE_INLINE
void Godunov_plm_fpu_z (const int i, const int j, const int k, const int n,
                        const amrex::Real dt, const amrex::Real dz,
                        amrex::Real& Im, amrex::Real& Ip,
                        const amrex::Array4<const amrex::Real> &S,
                        const amrex::Real& wmac,
                        const amrex::BCRec bc,
                        const int domain_klo, const int domain_khi, bool is_velocity = false)
{
    using namespace amrex;
    {
        bool extdir_or_ho_klo = (bc.lo(2) == BCType::ext_dir) or
                                (bc.lo(2) == BCType::hoextrap);
        bool extdir_or_ho_khi = (bc.hi(2) == BCType::ext_dir) or
                                (bc.hi(2) == BCType::hoextrap);

        Real wpls, wmns;

        if (k == domain_klo && (bc.lo(2) == BCType::ext_dir)) {
            wmns = S(i,j,k-1,n);
            if(n==2 and is_velocity)
            {
               wpls = S(i,j,k-1,n);
            } else {
               wpls = S(i,j,k  ,n) + 0.5 * (-1.0 - wmac * dt/dz) *
                  incflo_ho_zslope_extdir(i,j,k  ,n,S, extdir_or_ho_klo, extdir_or_ho_khi, domain_klo, domain_khi);
            }
        } else if (k == domain_khi+1 && (bc.hi(2) == BCType::ext_dir)) {
            wpls = S(i,j,k  ,n);
            if(n==2 and is_velocity)
            {
                wmns = S(i,j,k  ,n);
            } else {
               wmns = S(i,j,k-1,n) + 0.5 * ( 1.0 - wmac * dt/dz) *
                 incflo_ho_zslope_extdir(i,j,k-1,n,S, extdir_or_ho_klo, extdir_or_ho_khi, domain_klo, domain_khi);
            }
        } else {
            wpls = S(i,j,k  ,n) + 0.5 * (-1.0 - wmac * dt/dz) *
               incflo_ho_zslope_extdir(i,j,k  ,n,S, extdir_or_ho_klo, extdir_or_ho_khi, domain_klo, domain_khi);
            wmns = S(i,j,k-1,n) + 0.5 * ( 1.0 - wmac * dt/dz) *
               incflo_ho_zslope_extdir(i,j,k-1,n,S, extdir_or_ho_klo, extdir_or_ho_khi, domain_klo, domain_khi);
        }


        Ip = wmns;
        Im = wpls;
    }
}
#endif

}
#endif 
