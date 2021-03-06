//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#include <djvAV/Color.h>
#include <djvAV/ImageData.h>

#include <djvCore/ISystem.h>

#include <glm/mat4x4.hpp>

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            //! This class provides the base functionality for lights.
            class ILight : public std::enable_shared_from_this<ILight>
            {
                DJV_NON_COPYABLE(ILight);

            protected:
                ILight();

            public:
                virtual ~ILight() = 0;

                float getIntensity() const;

                void setIntensity(float);

            private:
                float _intensity = 1.F;
            };

            //! This class provides a hemisphere light.
            class HemisphereLight : public ILight
            {
            protected:
                HemisphereLight();

            public:
                static std::shared_ptr<HemisphereLight> create();

                const glm::vec3& getUp() const;
                const AV::Image::Color& getTopColor() const;
                const AV::Image::Color& getBottomColor() const;

                void setUp(const glm::vec3&);
                void setTopColor(const AV::Image::Color&);
                void setBottomColor(const AV::Image::Color&);

            private:
                glm::vec3 _up = glm::vec3(0.F, 1.F, 0.F);
                AV::Image::Color _topColor = AV::Image::Color::RGB_F32(1.F, .8F, .7F);
                AV::Image::Color _bottomColor = AV::Image::Color::RGB_F32(.4F, .4F, .8F);
            };

            //! This class provides a directional light.
            class DirectionalLight : public ILight
            {
            protected:
                DirectionalLight();

            public:
                static std::shared_ptr<DirectionalLight> create();

                const glm::vec3& getDirection() const;

                void setDirection(const glm::vec3&);

            private:
                glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
            };

            //! This class provides a point light.
            class PointLight : public ILight
            {
            protected:
                PointLight();

            public:
                static std::shared_ptr<PointLight> create();

                const glm::vec3& getPosition() const;

                void setPosition(const glm::vec3&);

            private:
                glm::vec3 _position = glm::vec3(0.F, 0.F, 0.F);
            };

            //! This class provides a spot light.
            class SpotLight : public ILight
            {
            protected:
                SpotLight();

            public:
                static std::shared_ptr<SpotLight> create();

                float getConeAngle() const;
                const glm::vec3& getDirection() const;
                const glm::vec3& getPosition() const;

                void setConeAngle(float);
                void setDirection(const glm::vec3&);
                void setPosition(const glm::vec3&);

            private:
                float _coneAngle = 90.F;
                glm::vec3 _direction = glm::vec3(-1.F, -1.F, -1.F);
                glm::vec3 _position = glm::vec3(0.F, 0.F, 0.F);
            };

        } // namespace Render3D
    } // namespace AV
} // namespace djv

#include <djvAV/Render3DLightInline.h>
