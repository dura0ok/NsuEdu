CREATE OR REPLACE FUNCTION apply_discount(current_price NUMERIC, discount_percent NUMERIC)
    RETURNS NUMERIC AS
$$
DECLARE
    new_price NUMERIC;
BEGIN
    IF discount_percent <= 0 THEN
        RETURN current_price;
    END IF;
    new_price := current_price * (1 - discount_percent / 100);
    RETURN new_price;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_print_cost(frames JSONB)
    RETURNS NUMERIC AS
$$
DECLARE
    total_price     NUMERIC := 0;
    discount_amount NUMERIC := 0;
    frame_count     INTEGER := 0;
    element         JSONB;
BEGIN
    -- Calculate the frame count by summing up the amount values in the JSONB array
    FOR i IN 0..(jsonb_array_length(frames) - 1)
        LOOP
            element := frames -> i;
            frame_count := frame_count + (element ->> 'amount')::INTEGER;
        END LOOP;

    RAISE NOTICE 'FRAME COUNT %', frame_count;

    FOR i IN 0..(jsonb_array_length(frames) - 1)
        LOOP
            element := frames -> i;
            total_price := total_price + (
                (element ->> 'amount')::INTEGER *
                COALESCE(
                        (SELECT price FROM print_prices WHERE id = (element ->> 'price_id')::BIGINT),
                        0
                )
                );
        END LOOP;


    RAISE NOTICE 'BEFORE DISCOUNT % FRAME COUNT %', total_price, frame_count;
    -- Get the discount amount
    SELECT discount INTO discount_amount FROM print_discounts WHERE id = get_discount_id(frame_count);
    if discount_amount IS NULL THEN
        discount_amount := 0;
    END IF;

    RAISE NOTICE 'DISC ID %, DISC %', get_discount_id(frame_count), discount_amount;

    total_price := apply_discount(total_price, discount_amount);
    RETURN total_price;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_print_orders_price(order_id BIGINT)
    RETURNS NUMERIC AS
$$
DECLARE
    frames_elements JSONB;
BEGIN
    SELECT jsonb_agg(
                   jsonb_build_object(
                           'amount', f.amount,
                           'price_id', f.print_price_id
                   )
           )
    INTO frames_elements
    FROM frames f
             JOIN print_orders po ON f.print_order_id = po.id
    WHERE po.order_id = calculate_print_orders_price.order_id;

    IF frames_elements IS NULL THEN
        RAISE NOTICE 'No frames found for order_id %', order_id;
        RETURN 0;
    END IF;

    RAISE NOTICE 'JSON %', frames_elements;
    RETURN calculate_print_cost(frames_elements);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_service_type_cost(service_elements JSONB)
    RETURNS NUMERIC AS
$$
DECLARE
    total_price     NUMERIC := 0;
    item            JSONB;
    cur_code        varchar;
    service_type_id BIGINT;
    amount          INTEGER;
    cur_price       NUMERIC;
    outlet_id       BIGINT;
BEGIN
    -- Extract outlet_id
    outlet_id := (service_elements ->> 'outlet_id')::BIGINT;

    -- Loop through each element in the services array
    FOR item IN SELECT * FROM jsonb_array_elements(service_elements -> 'services')
        LOOP
            -- Extract service_type_id and amount
            service_type_id := (item ->> 'service_type_id')::BIGINT;
            amount := (item ->> 'amount')::INTEGER;

            -- Debugging RAISE NOTICE
            RAISE NOTICE 'SERVICE TYPE ID %', service_type_id;

            -- Check for specific service type condition
            IF (SELECT LOWER(st.name) FROM service_types st WHERE st.id = service_type_id) =
               LOWER('Проявка Плёнки') THEN
                RAISE NOTICE 'Проявка плёнки';
                FOR cur_code IN SELECT * FROM jsonb_array_elements_text(item -> 'codes')
                    LOOP
                        RAISE NOTICE 'Code: % ot_id %', cur_code, outlet_id;
                        IF EXISTS(SELECT 1
                                  FROM films f
                                           JOIN service_orders so ON so.id = f.service_order_id
                                           JOIN orders o ON so.order_id = o.id
                                  WHERE f.code = cur_code
                                    AND o.accept_outlet_id = outlet_id) THEN
                            RAISE NOTICE '=====';
                            amount := amount - 1;
                        END IF;
                    END LOOP;
            END IF;

            -- Get the price for the service type
            SELECT price
            INTO cur_price
            FROM service_types
            WHERE id = service_type_id;

            -- Calculate the total price
            total_price := total_price + (COALESCE(cur_price, 0) * amount);
        END LOOP;

    RETURN total_price;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_service_orders_price(order_id BIGINT)
    RETURNS NUMERIC AS
$$
DECLARE
    items JSONB;
BEGIN
    SELECT jsonb_build_object(
                   'services', COALESCE(
                    (SELECT jsonb_agg(
                                    jsonb_build_object(
                                            'amount', so.count,
                                            'service_type_id', st.id
                                    )
                            )
                     FROM service_orders so
                              JOIN public.service_types st ON so.service_type_id = st.id
                     WHERE so.order_id = calculate_service_orders_price.order_id), '[]'::jsonb
                               ),
                   'outlet_id', (SELECT accept_outlet_id FROM orders WHERE id = order_id)
           )
    INTO items;

    RAISE NOTICE 'items %', items;
    RETURN calculate_service_type_cost(items);
END;
$$ LANGUAGE plpgsql;



CREATE OR REPLACE FUNCTION calculate_sale_orders_cost(item_elements JSONB)
    RETURNS NUMERIC AS
$$
DECLARE
    total_price NUMERIC := 0;
    item        JSONB;
    item_id     BIGINT;
    amount      INTEGER;
    cur_price   NUMERIC;
BEGIN
    FOR item IN SELECT * FROM jsonb_array_elements(item_elements)
        LOOP
            item_id := (item ->> 'item_id')::BIGINT;
            RAISE NOTICE 'ITEM ID %', item_id;
            amount := (item ->> 'amount')::INTEGER;

            SELECT price
            INTO cur_price
            FROM items
            where id = item_id;

            total_price := total_price + (COALESCE(cur_price, 0) * amount);
        END LOOP;

    RETURN total_price;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_sale_orders_price(order_id BIGINT)
    RETURNS NUMERIC AS
$$
DECLARE
    items JSONB;
BEGIN
   SELECT jsonb_agg(
                   jsonb_build_object(
                           'amount', so.amount,
                           'item_id', so.item_id
                   )
           )
    INTO items
    FROM sale_orders so
             where so.order_id = calculate_sale_orders_price.order_id;
    RAISE NOTICE 'ITEMS TO SALE %', items;
    RETURN calculate_sale_orders_cost(items);
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION update_order_price(order_id BIGINT, new_price NUMERIC)
    RETURNS VOID AS
$$
BEGIN
    UPDATE orders
    SET total_amount_price = new_price
    WHERE id = update_order_price.order_id;
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION get_client_discount(client_id BIGINT)
    RETURNS NUMERIC AS
$$
DECLARE
    client_discount INTEGER := 0;
BEGIN
    SELECT discount INTO client_discount FROM clients where id = client_id;
    RETURN client_discount;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION take_into_urgency_to_cost(is_urgent BOOLEAN, total_cost NUMERIC)
    RETURNS NUMERIC AS
$$
BEGIN
    IF is_urgent THEN
        RETURN total_cost * 2;
    END IF;
    RETURN total_cost;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION calculate_order_price(order_id BIGINT)
    RETURNS NUMERIC AS
$$
DECLARE
    total_price     INTEGER := 0;
    order_client_id BIGINT;
    order_urgency   boolean;
BEGIN
    PERFORM 1 FROM orders WHERE id = order_id;
    IF NOT FOUND THEN
        RAISE NOTICE 'Order ID % does not exist.', order_id;
        RETURN 0;
    END IF;

    SELECT calculate_print_orders_price(order_id) + total_price INTO total_price;
    RAISE NOTICE 'After print orders calculation %', total_price;

    SELECT calculate_service_orders_price(order_id) + total_price INTO total_price;
    RAISE NOTICE 'After service orders calculation %', total_price;

    SELECT calculate_sale_orders_price(order_id) + total_price INTO total_price;
    RAISE NOTICE 'After sale orders calculation %', total_price;

    SELECT client_id, is_urgent INTO order_client_id, order_urgency FROM orders where id = order_id;
    total_price := take_into_urgency_to_cost(order_urgency, total_price);
    RAISE NOTICE 'BEFORE DISCOUNT %, client_id %, disc % urgency %', total_price, order_client_id, get_client_discount(order_client_id), order_urgency;

    total_price := apply_discount(total_price, get_client_discount(order_client_id));
    RETURN total_price;
END;
$$ LANGUAGE plpgsql;


-- For service_orders table
CREATE OR REPLACE FUNCTION update_price_trigger()
    RETURNS TRIGGER AS
$$
BEGIN
    PERFORM update_order_price(NEW.order_id, calculate_order_price(NEW.order_id));
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- Create triggers on all related orders tables to call the update_price_trigger function


CREATE OR REPLACE TRIGGER update_price_trigger_for_orders
    AFTER INSERT OR UPDATE OR DELETE
    ON public.sale_orders
    FOR EACH ROW
EXECUTE FUNCTION update_price_trigger();

CREATE OR REPLACE TRIGGER update_price_trigger_for_print_orders
    AFTER INSERT OR UPDATE OR DELETE
    ON public.print_orders
    FOR EACH ROW
EXECUTE FUNCTION update_price_trigger();

CREATE OR REPLACE TRIGGER update_price_trigger_for_service_orders
    AFTER INSERT OR UPDATE OR DELETE
    ON public.service_orders
    FOR EACH ROW
EXECUTE FUNCTION update_price_trigger();