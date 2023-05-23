#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class PhysicalDevice
    {

        public:

            class Properties
            {

                public:

                    Properties(VkPhysicalDeviceProperties&& properties);

                    Properties(Properties const&) = delete;

                    Properties(Properties&&) = default;

                    ~Properties() = default;

                    Properties& operator=(Properties const&) = delete;

                    Properties& operator=(Properties&&) = default;

                private:

                    VkPhysicalDeviceProperties _properties;
            };


            PhysicalDevice() = default;

            PhysicalDevice(VkPhysicalDevice physical_device);

            PhysicalDevice(PhysicalDevice const&) = delete;

            PhysicalDevice(PhysicalDevice&&) = default;

            ~PhysicalDevice() = default;

            PhysicalDevice& operator=(PhysicalDevice const&) = delete;

            PhysicalDevice& operator=(PhysicalDevice&&) = default;

            Properties properties() const;

        private:

            static_assert(std::is_pointer_v<VkPhysicalDevice>);

            VkPhysicalDevice _physical_device;
    };

}  // namespace lue::vulkan
