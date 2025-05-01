#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

#include "event.h"
#include "graphics/maths_headers.h"
#include "logging/logging.h"

namespace phenyl::graphics {
    namespace detail {
        class IUIEventListeners {
        public:
            virtual ~IUIEventListeners () = default;

            virtual bool handle (const UIEvent& event) = 0;
        };

        template <typename T>
        class UIEventListeners : public IUIEventListeners {
        private:
            std::vector<std::function<bool(const T&)>> listeners;

        public:
            void addHandler (std::function<bool(const T&)> listener) {
                listeners.emplace_back(std::move(listener));
            }

            bool handle (const UIEvent& event) override {
                PHENYL_DASSERT(event.is<T>());

                const T& typedEvent = *event.get<T>();
                bool result = false;
                for (auto& i : listeners) {
                    const bool funcResult = i(typedEvent);
                    result = result || funcResult;
                }

                return result;
            }
        };
    }
    class Canvas;
    class UIEvent;

    struct Modifier {
        glm::vec2 minSize = {0.0f, 0.0f};
        std::optional<float> maxWidth = std::nullopt;
        std::optional<float> maxHeight = std::nullopt;
        float padding = 0.0f;
        float weight = 0.0f;
        glm::vec2 offset = {0.0f, 0.0f};

        [[nodiscard]] Modifier withSize (glm::vec2 size) const noexcept {
            Modifier copy = *this;
            copy.minSize = size;
            copy.maxWidth = size.x;
            copy.maxHeight = size.y;

            return copy;
        }

        [[nodiscard]] Modifier withSize (glm::vec2 minSize, glm::vec2 maxSize) const noexcept {
            Modifier copy = *this;
            copy.minSize = minSize;
            copy.maxWidth = maxSize.x;
            copy.maxHeight = maxSize.y;

            return copy;
        }

        [[nodiscard]] Modifier withPadding (float padding) const noexcept {
            Modifier copy = *this;
            copy.padding = padding;

            return copy;
        }

        [[nodiscard]] Modifier withWeight (float weight) const noexcept {
            Modifier copy = *this;
            copy.weight = weight;

            return copy;
        }

        [[nodiscard]] Modifier withOffset (glm::vec2 offset) const noexcept {
            Modifier copy = *this;
            copy.offset = offset;

            return copy;
        }
    };

    struct WidgetConstraints {
        std::optional<float> minWidth;
        std::optional<float> minHeight;
        glm::vec2 maxSize;
    };


    class Widget {
    private:
        Modifier m_modifier;
        Widget* m_parent = nullptr;
        glm::vec2 m_size = {0, 0};
        glm::vec2 m_offset = {0, 0};
        std::optional<glm::vec2> m_oldPointerPos = std::nullopt;
        std::unordered_map<std::size_t, std::unique_ptr<detail::IUIEventListeners>> m_listeners;

        bool bubbleUp (const UIEvent& event);

        template <typename T>
        detail::UIEventListeners<T>& getListeners () {
            auto typeIndex = meta::type_index<T>();
            auto it = m_listeners.find(typeIndex);
            if (it == m_listeners.end()) {
                it = m_listeners.emplace(typeIndex, std::make_unique<detail::UIEventListeners<T>>()).first;
            }

            return static_cast<detail::UIEventListeners<T>&>(*it->second);
        }
    protected:
        bool handle (const UIEvent& event);
        virtual void queueChildDestroy (Widget* child);
        void setModifier (const Modifier& modifier);
        void setDimensions (glm::vec2 newDims);
    public:
        explicit Widget (const Modifier& modifier = Modifier{});
        Widget (const Widget&) = delete;
        Widget (Widget&&) = default;

        Widget& operator= (const Widget&) = delete;
        Widget& operator= (Widget&&) = default;

        virtual ~Widget ();

        virtual Widget* pick (glm::vec2 pointer) noexcept;
        virtual bool pointerUpdate (glm::vec2 pointer);
        virtual void pointerLeave ();

        virtual void measure (const WidgetConstraints& constraints);
        virtual void render (Canvas& canvas) = 0;

        virtual void update ();
        void queueDestroy ();

        [[nodiscard]] Widget* parent () const noexcept {
            return m_parent;
        }
        void setParent (Widget* parent);
        virtual void setOffset (glm::vec2 newOffset);

        [[nodiscard]] const Modifier& modifier () const noexcept {
            return m_modifier;
        }

        glm::vec2 offset () const noexcept {
            return m_offset;
        }

        glm::vec2 dimensions () const noexcept {
            return m_size;
        }

        void raise (const UIEvent& event);
        [[nodiscard]] bool pointerOver () const noexcept {
            return static_cast<bool>(m_oldPointerPos);
        }

        template <typename T>
        void addListener (std::function<void(const T&)>&& listener) {
            addListener([func = std::move(listener)] (const T& event) {
                func(event);
                return false;
            });
        }

        template <typename T>
        void addListener (std::function<bool(const T&)>&& listener) {
            getListeners<T>().addHandler(std::move(listener));
        }

        template <typename F>
        void addListener (F&& f) {
            addListener(std::function{std::forward<F>(f)});
        }
    };
}
