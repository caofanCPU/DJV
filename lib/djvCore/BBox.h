//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#pragma once

#include <djvCore/Vector.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace Core
    {
        //! This namespace provides bounding box functionality.
        namespace BBox
        {
            //! This struct provides an axis aligned bounding box.
            template<typename T, glm::precision P = glm::defaultp>
            struct tBBox2
            {
                constexpr tBBox2();
                constexpr tBBox2(const glm::tvec2<T, P> &);
                constexpr tBBox2(const glm::tvec2<T, P> & min, const glm::tvec2<T, P> & max);
                constexpr tBBox2(T x, T y, T w, T h);

                //! \name Box Components
                ///@{

                glm::tvec2<T, P> min;
                glm::tvec2<T, P> max;

                constexpr T x() const;
                constexpr T y() const;
                constexpr T w() const;
                constexpr T h() const;

                ///@}

                //! \name Utilities
                ///@{

                constexpr bool isValid() const;
                constexpr glm::tvec2<T, P> getSize() const;
                constexpr glm::tvec2<T, P> getCenter() const;
                constexpr T getArea() const;
                inline float getAspect() const;
                inline void zero();

                constexpr bool contains(const tBBox2<T, P> &) const;
                constexpr bool contains(const glm::tvec2<T, P> &) const;

                constexpr bool intersects(const tBBox2<T, P> &) const;
                inline tBBox2<T, P> intersect(const tBBox2<T, P> &) const;

                inline void expand(const tBBox2<T, P> &);
                inline void expand(const glm::tvec2<T, P> &);

                constexpr tBBox2<T, P> margin(const glm::tvec2<T, P> &) const;
                constexpr tBBox2<T, P> margin(T) const;
                constexpr tBBox2<T, P> margin(T x0, T y0, T x1, T y1) const;

                ///@}

                constexpr bool operator == (const tBBox2<T, P> &) const;
                constexpr bool operator != (const tBBox2<T, P> &) const;
            };

            //! This struct provides an axis aligned bounding box.
            template<typename T, glm::precision P = glm::defaultp>
            struct tBBox3
            {
                constexpr tBBox3();
                constexpr tBBox3(const glm::tvec3<T, P> &);
                constexpr tBBox3(const glm::tvec3<T, P> & min, const glm::tvec3<T, P> & max);
                constexpr tBBox3(T x, T y, T z, T w, T h, T d);

                //! \name Components
                ///@{

                glm::tvec3<T, P> min, max;

                constexpr T x() const;
                constexpr T y() const;
                constexpr T z() const;
                constexpr T w() const;
                constexpr T h() const;
                constexpr T d() const;

                ///@}

                //! \name Box Utilities
                ///@{

                constexpr bool isValid() const;
                constexpr glm::tvec3<T, P> getSize() const;
                constexpr glm::tvec3<T, P> getCenter() const;
                inline void zero();

                constexpr bool contains(const tBBox3<T, P> &) const;
                constexpr bool contains(const glm::tvec3<T, P> &) const;

                constexpr bool intersects(const tBBox3<T, P> &) const;
                inline tBBox3<T, P> intersect(const tBBox3<T, P> &) const;
                inline bool intersect(const glm::tvec3<T, P> & start, const glm::tvec3<T, P> & end, glm::tvec3<T, P> & out) const;

                inline void expand(const tBBox3<T, P> &);
                inline void expand(const glm::tvec3<T, P> &);

                ///@}

                constexpr bool operator == (const tBBox3<T, P> &) const;
                constexpr bool operator != (const tBBox3<T, P> &) const;
            };

            typedef tBBox2<int, glm::lowp>      BBox2i_lowp;
            typedef tBBox2<int, glm::mediump>   BBox2i_mediump;
            typedef tBBox2<int, glm::highp>     BBox2i_highp;

            typedef tBBox2<float, glm::lowp>    BBox2f_lowp;
            typedef tBBox2<float, glm::mediump> BBox2f_mediump;
            typedef tBBox2<float, glm::highp>   BBox2f_highp;

            typedef tBBox3<float, glm::lowp>    BBox3f_lowp;
            typedef tBBox3<float, glm::mediump> BBox3f_mediump;
            typedef tBBox3<float, glm::highp>   BBox3f_highp;

#if(defined(GLM_PRECISION_LOWP_INT))
            typedef BBox2i_lowp    BBox2i;
#elif(defined(GLM_PRECISION_MEDIUMP_INT))
            typedef BBox2i_mediump BBox2i;
#else //defined(GLM_PRECISION_HIGHP_INT)
            typedef BBox2i_highp   BBox2i;
#endif //GLM_PRECISION

#if(defined(GLM_PRECISION_LOWP_FLOAT))
            typedef BBox2f_lowp    BBox2f;
            typedef BBox3f_lowp    BBox3f;
#elif(defined(GLM_PRECISION_MEDIUMP_FLOAT))
            typedef BBox2f_mediump BBox2f;
            typedef BBox3f_mediump BBox3f;
#else //defined(GLM_PRECISION_HIGHP_FLOAT)
            typedef BBox2f_highp   BBox2f;
            typedef BBox3f_highp   BBox3f;
#endif //GLM_PRECISION

        } // namespace BBox

        using BBox::BBox2i;
        using BBox::BBox2f;
        using BBox::BBox3f;

    } // namespace Core

    template<typename T, glm::precision P = glm::defaultp>
    inline bool fuzzyCompare(const Core::BBox::tBBox2<T, P> &, const Core::BBox::tBBox2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    inline bool fuzzyCompare(const Core::BBox::tBBox3<T, P> &, const Core::BBox::tBBox3<T, P> &);

    template<typename T, glm::precision P = glm::defaultp>
    inline Core::BBox::tBBox3<T, P> operator * (const Core::BBox::tBBox3<T, P> &, const glm::mat4 &);

    template<typename T, glm::precision P = glm::defaultp>
    inline std::ostream & operator << (std::ostream &, const Core::BBox::tBBox2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    inline std::ostream & operator << (std::ostream &, const Core::BBox::tBBox3<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    inline std::istream & operator >> (std::istream &, Core::BBox::tBBox2<T, P> &);
    template<typename T, glm::precision P = glm::defaultp>
    inline std::istream & operator >> (std::istream &, Core::BBox::tBBox3<T, P> &);

} // namespace djv

#include <djvCore/BBoxInline.h>