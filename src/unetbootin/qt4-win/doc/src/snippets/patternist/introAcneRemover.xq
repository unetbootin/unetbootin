<result>
    <para>The following Acne removers have shipped, ordered by shipping date(oldest first):</para>
    {
        for $i in doc("myOrders.xml")/orders/order[@product = "Acme's Acne Remover"]
        order by xs:date($i/@shippingDate) descending
        return $i
    }
</result>
