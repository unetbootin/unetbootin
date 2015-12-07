for $item in doc('feed.rss')//item
return <p>
        {
            $item/description/node()
        }
       </p>
