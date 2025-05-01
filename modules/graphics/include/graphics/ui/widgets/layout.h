#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "widget.h"

namespace phenyl::graphics {
    enum class LayoutArrangement {
        START,
        END,
        CENTER,
        SPACED
    };

    enum class LayoutAlignment {
        START,
        END,
        CENTER
    };

    class LayoutWidget : public Widget {
    public:
        enum class Axis {
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        LayoutWidget (Axis axis, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier = {});

        template <std::derived_from<Widget> T>
        T* insert (std::size_t pos, T&& widget) {
            return static_cast<T*>(insert(pos, std::make_unique<T>(std::forward<T>(widget))));
        }
        template <std::derived_from<Widget> T, typename ...Args>
        T* emplace (std::size_t pos, Args&&... args) requires std::constructible_from<T, Args&&...> {
            return static_cast<T*>(insert(pos, std::make_unique<T>(std::forward<Args>(args)...)));
        }

        template <std::derived_from<Widget> T>
        T* pushBack (T&& widget) {
            return static_cast<T*>(pushBack(std::make_unique<T>(std::forward<T>(widget))));
        }
        template <std::derived_from<Widget> T, typename ...Args>
        T* emplaceBack (Args&&... args) requires std::constructible_from<T, Args&&...> {
            return static_cast<T*>(pushBack(std::make_unique<T>(std::forward<Args>(args)...)));
        }

        [[nodiscard]] Widget* at (std::size_t index) const;
        std::size_t size () const noexcept;
        bool empty () const noexcept;

        void clear ();

        Widget* pick (glm::vec2 pointer) noexcept override;
        void setOffset (glm::vec2 newOffset) override;
        void update () override;
        void measure (const WidgetConstraints& constraints) override;
        void render (Canvas& canvas) override;
        void queueChildDestroy (Widget* child) override;
        bool pointerUpdate (glm::vec2 pointer) override;
        void pointerLeave () override;

    private:
        Axis m_axis;
        LayoutArrangement m_arrangement;
        LayoutAlignment m_alignment;
        std::vector<std::unique_ptr<Widget>> m_children;
        std::vector<glm::vec2> m_childOff;
        std::unordered_set<Widget*> m_widgetsToDelete;

        float measureChildren (glm::vec2 dims, glm::vec2 mainAxis, glm::vec2 secondAxis);
        Widget* insert (std::size_t pos, std::unique_ptr<Widget> child);
        Widget* pushBack (std::unique_ptr<Widget> child);
    };

    enum class ColumnDirection {
        UP,
        DOWN
    };

    class ColumnWidget : public LayoutWidget {
    public:
        ColumnWidget (ColumnDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier);

        template <std::derived_from<Widget> ...Args>
        explicit ColumnWidget (const Modifier& modifier, Args&&... args) : ColumnWidget{ColumnDirection::DOWN, LayoutArrangement::START, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args>
        explicit ColumnWidget (ColumnDirection dir, const Modifier& modifier, Args&&... args) : ColumnWidget{dir, LayoutArrangement::START, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args>
        ColumnWidget (ColumnDirection dir, LayoutArrangement arrangement, const Modifier& modifier, Args&&... args) : ColumnWidget{dir, arrangement, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args> requires (sizeof...(Args) > 0)
        ColumnWidget (ColumnDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier, Args&&... args) : ColumnWidget{dir, arrangement, alignment, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }
    };

    enum class RowDirection {
        LEFT,
        RIGHT
    };

    class RowWidget : public LayoutWidget {
    public:
        RowWidget (RowDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier);

        template <std::derived_from<Widget> ...Args>
        explicit RowWidget (const Modifier& modifier, Args&&... args) : RowWidget{RowDirection::LEFT, LayoutArrangement::START, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args>
        explicit RowWidget (RowDirection dir, const Modifier& modifier, Args&&... args) : RowWidget{dir, LayoutArrangement::START, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args>
        RowWidget (RowDirection dir, LayoutArrangement arrangement, const Modifier& modifier, Args&&... args) : RowWidget{dir, arrangement, LayoutAlignment::START, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }

        template <std::derived_from<Widget> ...Args> requires (sizeof...(Args) > 0)
        RowWidget (RowDirection dir, LayoutArrangement arrangement, LayoutAlignment alignment, const Modifier& modifier, Args&&... args) : RowWidget{dir, arrangement, alignment, modifier} {
            ([&] {
                pushBack(args);
            } (), ...);
        }
    };
}
