//! [0]
        int MyLayout::heightForWidth(int w) const
        {
            if (cache_dirty || cached_width != w) {
                // not all C++ compilers support "mutable"
                MyLayout *that = (MyLayout*)this;
                int h = calculateHeightForWidth(w);
                that->cached_hfw = h;
                return h;
            }
            return cached_hfw;
        }
//! [0]


