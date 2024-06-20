--
-- PostgreSQL database dump
--

-- Dumped from database version 16.2 (Debian 16.2-1.pgdg120+2)
-- Dumped by pg_dump version 16.2 (Debian 16.2-1.pgdg120+2)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: db_dev; Type: DATABASE; Schema: -; Owner: durachok
--

CREATE DATABASE db_dev WITH TEMPLATE = template0 ENCODING = 'UTF8' LOCALE_PROVIDER = libc LOCALE = 'en_US.utf8';


ALTER DATABASE db_dev OWNER TO durachok;

\connect db_dev

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: public; Type: SCHEMA; Schema: -; Owner: pg_database_owner
--

CREATE SCHEMA public;


ALTER SCHEMA public OWNER TO pg_database_owner;

--
-- Name: SCHEMA public; Type: COMMENT; Schema: -; Owner: pg_database_owner
--

COMMENT ON SCHEMA public IS 'standard public schema';


--
-- Name: apply_discount(numeric, numeric); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.apply_discount(current_price numeric, discount_percent numeric) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
DECLARE
    new_price NUMERIC;
BEGIN
    IF discount_percent <= 0 THEN
        RETURN current_price;
    END IF;
    new_price := current_price * (1 - discount_percent / 100);
    RETURN new_price;
END;
$$;


ALTER FUNCTION public.apply_discount(current_price numeric, discount_percent numeric) OWNER TO durachok;

--
-- Name: auto_set_print_discount(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.auto_set_print_discount() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    frame_count     INTEGER;
    max_discount_id INTEGER;
BEGIN
    RAISE NOTICE '%', NEW;
    -- Получаем количество кадров для данного заказа
    SELECT SUM(frames.amount)
    INTO frame_count
    FROM frames
    WHERE print_order_id = NEW.print_order_id;

    -- Логируем количество кадров
    RAISE NOTICE 'Frame count for order % is %', NEW.print_order_id, frame_count;

    -- Получаем максимальный discount_id
    max_discount_id := get_discount_id(frame_count);

    RAISE NOTICE 'Max discount id is %', max_discount_id;

    UPDATE print_orders SET discount_id = max_discount_id WHERE id = NEW.print_order_id;

    RETURN NEW;
END;
$$;


ALTER FUNCTION public.auto_set_print_discount() OWNER TO durachok;

--
-- Name: calculate_order_price(bigint); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_order_price(order_id bigint) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_order_price(order_id bigint) OWNER TO durachok;

--
-- Name: calculate_print_cost(jsonb); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_print_cost(frames jsonb) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_print_cost(frames jsonb) OWNER TO durachok;

--
-- Name: calculate_print_orders_price(bigint); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_print_orders_price(order_id bigint) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_print_orders_price(order_id bigint) OWNER TO durachok;

--
-- Name: calculate_sale_orders_cost(jsonb); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_sale_orders_cost(item_elements jsonb) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_sale_orders_cost(item_elements jsonb) OWNER TO durachok;

--
-- Name: calculate_sale_orders_price(bigint); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_sale_orders_price(order_id bigint) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_sale_orders_price(order_id bigint) OWNER TO durachok;

--
-- Name: calculate_service_orders_price(bigint); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_service_orders_price(order_id bigint) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_service_orders_price(order_id bigint) OWNER TO durachok;

--
-- Name: calculate_service_type_cost(jsonb); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.calculate_service_type_cost(service_elements jsonb) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
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
$$;


ALTER FUNCTION public.calculate_service_type_cost(service_elements jsonb) OWNER TO durachok;

--
-- Name: check_and_update_storage(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_and_update_storage() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    SALE_STORAGE_ID    INTEGER;
    AVAILABLE_QUANTITY INTEGER;
BEGIN
    SELECT s.id
    INTO SALE_STORAGE_ID
    FROM storage s
             JOIN orders o ON o.id = NEW.order_id
    WHERE o.accept_outlet_id = s.outlet_id;

    RAISE NOTICE 'Sale storage ID: %', SALE_STORAGE_ID;

    SELECT quantity
    INTO AVAILABLE_QUANTITY
    FROM storage_items si
    WHERE si.item_id = NEW.item_id
      AND si.storage_id = SALE_STORAGE_ID;

    RAISE NOTICE 'Available quantity: %', AVAILABLE_QUANTITY;

    IF AVAILABLE_QUANTITY IS NULL OR AVAILABLE_QUANTITY < NEW.amount THEN
        RAISE EXCEPTION 'Недостаточное количество товара на складе % Доступно %',
            (select product_name from items where id = NEW.item_id), AVAILABLE_QUANTITY;
    END IF;

    UPDATE storage_items
    SET quantity = quantity - NEW.amount
    WHERE item_id = NEW.item_id
      AND storage_id = SALE_STORAGE_ID;

    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_and_update_storage() OWNER TO durachok;

--
-- Name: check_film_development_order(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_film_development_order() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    type_id BIGINT;
BEGIN
    SELECT id INTO type_id FROM service_types WHERE name = 'Проявка плёнки';
    IF NOT EXISTS (SELECT 1 FROM service_orders WHERE service_type_id = type_id AND id = NEW.service_order_id) THEN
        RAISE EXCEPTION 'Тип Услуги должен быть проявка плёнки';
    END IF;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_film_development_order() OWNER TO durachok;

--
-- Name: check_main_storage(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_main_storage() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM storage WHERE id = NEW.storage_id AND is_main_storage = TRUE) THEN
        RAISE EXCEPTION 'The storage specified is not marked as main storage.';
    END IF;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_main_storage() OWNER TO durachok;

--
-- Name: check_material_availability(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_material_availability() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    required_materials RECORD;
    available_quantity INTEGER;
BEGIN
    FOR required_materials IN
        SELECT item_id, count
        FROM service_types_needed_items
        WHERE service_type_id = NEW.service_type_id
        LOOP
        --         RAISE NOTICE 'ORDER_ID = %', NEW.order_id;
--         RAISE NOTICE 'ITEM_REQUIRED = %', required_materials.item_id;
--         RAISE NOTICE 'ITEM_REQUIRED = %', required_materials.count;
            SELECT quantity
            INTO available_quantity
            FROM storage_items si
                     JOIN storage s ON s.outlet_id = (SELECT accept_outlet_id FROM orders WHERE id = NEW.order_id)
            WHERE si.item_id = required_materials.item_id
              AND si.storage_id = s.id;


            IF available_quantity IS NULL THEN
                available_quantity := 0;
            END IF;

            IF available_quantity < required_materials.count * NEW.count THEN
                RAISE EXCEPTION 'Недостаточно материалов для производства услуги %. Не хватает товара %, Нужно %, Доступно %',
                    (SELECT name from service_types where id = NEW.service_type_id),
                    (select product_name from items where id = required_materials.item_id),
                    required_materials.count,
                    available_quantity;
            ELSE
                RAISE NOTICE 'Material available on the storage for this service order. Item ID: %, Required Count: %, Available Count: %', required_materials.item_id, required_materials.count * NEW.count, available_quantity;

                UPDATE storage_items
                SET quantity = quantity - required_materials.count * NEW.count
                FROM storage s
                WHERE storage_items.storage_id = s.id
                  AND storage_items.item_id = required_materials.item_id;
            END IF;
        END LOOP;

    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_material_availability() OWNER TO durachok;

--
-- Name: check_outlet_service_type(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_outlet_service_type() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF EXISTS (SELECT 1
               FROM service_types_outlets sto
                        JOIN outlets o ON sto.outlet_type_id = o.type_id
                        JOIN orders ord ON o.id = ord.accept_outlet_id
               WHERE sto.service_type_id = NEW.service_type_id
                 AND ord.id = NEW.order_id) THEN
        RETURN NEW;
    ELSE
        RAISE EXCEPTION 'Выбранная торговая точка не может предоставить выбранный тип услуги';
    END IF;
END;
$$;


ALTER FUNCTION public.check_outlet_service_type() OWNER TO durachok;

--
-- Name: check_outlet_type_consistency(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_outlet_type_consistency() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    expected_outlet_type text;
BEGIN
    expected_outlet_type := TG_ARGV[0];

    IF NEW.outlet_id IS NOT NULL THEN
        IF (SELECT type_id FROM outlets WHERE id = NEW.outlet_id) !=
           (SELECT id FROM outlet_types WHERE name = expected_outlet_type) THEN
            RAISE EXCEPTION 'The type of building does not match the expected type: %', expected_outlet_type;
        END IF;
    END IF;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_outlet_type_consistency() OWNER TO durachok;

--
-- Name: check_storage_capacity(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_storage_capacity() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    total_quantity INTEGER;
    capacity_limit integer;
BEGIN

    SELECT COALESCE(SUM(quantity), 0)
    INTO total_quantity
    FROM storage_items
    WHERE storage_id = NEW.storage_id
      AND item_id != NEW.item_id;

    SELECT capacity INTO capacity_limit FROM storage WHERE id = NEW.storage_id;
    IF capacity_limit IS NULL THEN
        RAISE EXCEPTION 'Определите вместимость склада';
    end if;

    RAISE NOTICE 'Total loaded % Add capacity % Capacity limit %', total_quantity, NEW.quantity, capacity_limit;

    IF total_quantity + NEW.quantity > capacity_limit THEN
        RAISE EXCEPTION 'Хранилище с идентификатором % и ограничением емкости % превысило свою емкость на % единиц.',
            NEW.storage_id, capacity_limit, total_quantity + NEW.quantity - capacity_limit;
    END IF;

    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_storage_capacity() OWNER TO durachok;

--
-- Name: check_urgent_order_type(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_urgent_order_type() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF NEW.is_urgent THEN
        IF NOT EXISTS (SELECT 1
                       FROM outlets o
                                JOIN outlet_types ot ON o.type_id = ot.id
                       WHERE o.id = NEW.accept_outlet_id
                         AND ot.name = 'Филиал') THEN
            RAISE EXCEPTION 'Срочный заказ доступен только для филиалов';
        END IF;
    END IF;
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_urgent_order_type() OWNER TO durachok;

--
-- Name: check_vendor_item_exists(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.check_vendor_item_exists() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
DECLARE
    delivery_vendor_id INTEGER;
BEGIN
    -- Получаем vendor_id по delivery_id
    SELECT vendor_id
    INTO delivery_vendor_id
    FROM deliveries
    WHERE id = NEW.delivery_id;

    RAISE NOTICE 'Delivery Vendor ID: %', delivery_vendor_id;

    -- Проверяем, существует ли запись в таблице vendor_items
    IF NOT EXISTS (SELECT 1
                   FROM vendor_items
                   WHERE vendor_id = delivery_vendor_id
                     AND item_id = NEW.item_id) THEN
        RAISE EXCEPTION 'Поставщик не поставляет этот товар';
    END IF;

    RETURN NEW;
END;
$$;


ALTER FUNCTION public.check_vendor_item_exists() OWNER TO durachok;

--
-- Name: delete_outlet_on_junction_delete(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.delete_outlet_on_junction_delete() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF OLD.outlet_id IS NOT NULL THEN
        DELETE FROM outlets WHERE id = OLD.outlet_id;
    END IF;
    RETURN OLD;
END;
$$;


ALTER FUNCTION public.delete_outlet_on_junction_delete() OWNER TO durachok;

--
-- Name: get_client_discount(bigint); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.get_client_discount(client_id bigint) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
DECLARE
    client_discount INTEGER := 0;
BEGIN
    SELECT discount INTO client_discount FROM clients where id = client_id;
    RETURN client_discount;
END;
$$;


ALTER FUNCTION public.get_client_discount(client_id bigint) OWNER TO durachok;

--
-- Name: get_discount_id(integer); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.get_discount_id(frame_count integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
    max_discount_id INTEGER;
BEGIN
    SELECT id
    INTO max_discount_id
    FROM print_discounts
    WHERE frame_count >= photo_amount
    ORDER BY discount DESC
    LIMIT 1;

    RETURN max_discount_id;
END;
$$;


ALTER FUNCTION public.get_discount_id(frame_count integer) OWNER TO durachok;

--
-- Name: take_into_urgency_to_cost(boolean, numeric); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.take_into_urgency_to_cost(is_urgent boolean, total_cost numeric) RETURNS numeric
    LANGUAGE plpgsql
    AS $$
BEGIN
    IF is_urgent THEN
        RETURN total_cost * 2;
    END IF;
    RETURN total_cost;
END;
$$;


ALTER FUNCTION public.take_into_urgency_to_cost(is_urgent boolean, total_cost numeric) OWNER TO durachok;

--
-- Name: update_order_price(bigint, numeric); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.update_order_price(order_id bigint, new_price numeric) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
    UPDATE orders
    SET total_amount_price = new_price
    WHERE id = update_order_price.order_id;
END;
$$;


ALTER FUNCTION public.update_order_price(order_id bigint, new_price numeric) OWNER TO durachok;

--
-- Name: update_price_trigger(); Type: FUNCTION; Schema: public; Owner: durachok
--

CREATE FUNCTION public.update_price_trigger() RETURNS trigger
    LANGUAGE plpgsql
    AS $$
BEGIN
    PERFORM update_order_price(NEW.order_id, calculate_order_price(NEW.order_id));
    RETURN NEW;
END;
$$;


ALTER FUNCTION public.update_price_trigger() OWNER TO durachok;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: auth_group; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.auth_group (
    id integer NOT NULL,
    name character varying(150) NOT NULL
);


ALTER TABLE public.auth_group OWNER TO durachok;

--
-- Name: auth_group_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.auth_group ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.auth_group_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: auth_group_permissions; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.auth_group_permissions (
    id bigint NOT NULL,
    group_id integer NOT NULL,
    permission_id integer NOT NULL
);


ALTER TABLE public.auth_group_permissions OWNER TO durachok;

--
-- Name: auth_group_permissions_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.auth_group_permissions ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.auth_group_permissions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: auth_permission; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.auth_permission (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    content_type_id integer NOT NULL,
    codename character varying(100) NOT NULL
);


ALTER TABLE public.auth_permission OWNER TO durachok;

--
-- Name: auth_permission_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.auth_permission ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.auth_permission_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: branches; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.branches (
    outlet_id bigint NOT NULL
);


ALTER TABLE public.branches OWNER TO durachok;

--
-- Name: TABLE branches; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.branches IS 'Филиалы';


--
-- Name: COLUMN branches.outlet_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.branches.outlet_id IS 'Привязка к зданию';


--
-- Name: clients; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.clients (
    id bigint NOT NULL,
    full_name character varying(50) NOT NULL,
    is_professional boolean NOT NULL,
    discount smallint NOT NULL,
    CONSTRAINT clients_discount_check CHECK ((discount >= 0))
);


ALTER TABLE public.clients OWNER TO durachok;

--
-- Name: TABLE clients; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.clients IS 'Таблица клиентов различных фотоцентров';


--
-- Name: COLUMN clients.full_name; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.clients.full_name IS 'ФИО клиента';


--
-- Name: COLUMN clients.is_professional; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.clients.is_professional IS 'Профессионал или Любитель?';


--
-- Name: COLUMN clients.discount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.clients.discount IS 'Персональная скидка';


--
-- Name: clients_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.clients ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.clients_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: deliveries; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.deliveries (
    id bigint NOT NULL,
    delivery_date date NOT NULL,
    storage_id bigint NOT NULL,
    vendor_id bigint NOT NULL
);


ALTER TABLE public.deliveries OWNER TO durachok;

--
-- Name: TABLE deliveries; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.deliveries IS 'Поставки в главный склад';


--
-- Name: deliveries_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.deliveries ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.deliveries_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: delivery_items; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.delivery_items (
    id bigint NOT NULL,
    purchase_price numeric(15,2) NOT NULL,
    amount integer NOT NULL,
    delivery_id bigint NOT NULL,
    item_id bigint NOT NULL
);


ALTER TABLE public.delivery_items OWNER TO durachok;

--
-- Name: TABLE delivery_items; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.delivery_items IS 'Товары в поставке';


--
-- Name: COLUMN delivery_items.purchase_price; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.delivery_items.purchase_price IS 'Цена закупки товара';


--
-- Name: COLUMN delivery_items.amount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.delivery_items.amount IS 'Количество поставленных единиц товара';


--
-- Name: delivery_items_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.delivery_items ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.delivery_items_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: django_admin_log; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.django_admin_log (
    id integer NOT NULL,
    action_time timestamp with time zone NOT NULL,
    object_id text,
    object_repr character varying(200) NOT NULL,
    action_flag smallint NOT NULL,
    change_message text NOT NULL,
    content_type_id integer,
    user_id bigint NOT NULL,
    CONSTRAINT django_admin_log_action_flag_check CHECK ((action_flag >= 0))
);


ALTER TABLE public.django_admin_log OWNER TO durachok;

--
-- Name: django_admin_log_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.django_admin_log ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.django_admin_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: django_content_type; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.django_content_type (
    id integer NOT NULL,
    app_label character varying(100) NOT NULL,
    model character varying(100) NOT NULL
);


ALTER TABLE public.django_content_type OWNER TO durachok;

--
-- Name: django_content_type_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.django_content_type ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.django_content_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: django_migrations; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.django_migrations (
    id bigint NOT NULL,
    app character varying(255) NOT NULL,
    name character varying(255) NOT NULL,
    applied timestamp with time zone NOT NULL
);


ALTER TABLE public.django_migrations OWNER TO durachok;

--
-- Name: django_migrations_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.django_migrations ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.django_migrations_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: django_session; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.django_session (
    session_key character varying(40) NOT NULL,
    session_data text NOT NULL,
    expire_date timestamp with time zone NOT NULL
);


ALTER TABLE public.django_session OWNER TO durachok;

--
-- Name: entities_customuser; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.entities_customuser (
    id bigint NOT NULL,
    password character varying(128) NOT NULL,
    last_login timestamp with time zone,
    is_superuser boolean NOT NULL,
    username character varying(150) NOT NULL,
    first_name character varying(150) NOT NULL,
    last_name character varying(150) NOT NULL,
    email character varying(254) NOT NULL,
    is_staff boolean NOT NULL,
    is_active boolean NOT NULL,
    date_joined timestamp with time zone NOT NULL,
    outlet_id bigint
);


ALTER TABLE public.entities_customuser OWNER TO durachok;

--
-- Name: entities_customuser_groups; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.entities_customuser_groups (
    id bigint NOT NULL,
    customuser_id bigint NOT NULL,
    group_id integer NOT NULL
);


ALTER TABLE public.entities_customuser_groups OWNER TO durachok;

--
-- Name: entities_customuser_groups_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.entities_customuser_groups ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.entities_customuser_groups_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: entities_customuser_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.entities_customuser ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.entities_customuser_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: entities_customuser_user_permissions; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.entities_customuser_user_permissions (
    id bigint NOT NULL,
    customuser_id bigint NOT NULL,
    permission_id integer NOT NULL
);


ALTER TABLE public.entities_customuser_user_permissions OWNER TO durachok;

--
-- Name: entities_customuser_user_permissions_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.entities_customuser_user_permissions ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.entities_customuser_user_permissions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: film_development_orders; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.film_development_orders (
    id bigint NOT NULL,
    code character varying(255) NOT NULL,
    service_order_id bigint NOT NULL
);


ALTER TABLE public.film_development_orders OWNER TO durachok;

--
-- Name: TABLE film_development_orders; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.film_development_orders IS 'Проявка пленок(вложено в чек заказа)';


--
-- Name: COLUMN film_development_orders.code; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.film_development_orders.code IS 'Уникальный код пленки';


--
-- Name: COLUMN film_development_orders.service_order_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.film_development_orders.service_order_id IS 'Связь с заказом';


--
-- Name: film_development_orders_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.film_development_orders ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.film_development_orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: films; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.films (
    id bigint NOT NULL,
    code character varying(255) NOT NULL,
    service_order_id bigint
);


ALTER TABLE public.films OWNER TO durachok;

--
-- Name: films_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.films ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.films_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: firms; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.firms (
    id bigint NOT NULL,
    name character varying(100) NOT NULL
);


ALTER TABLE public.firms OWNER TO durachok;

--
-- Name: TABLE firms; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.firms IS 'Бренды товаров';


--
-- Name: COLUMN firms.name; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.firms.name IS 'Название Бренда';


--
-- Name: firms_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.firms ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.firms_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: frames; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.frames (
    id bigint NOT NULL,
    amount integer NOT NULL,
    code character varying(255) NOT NULL,
    frame_number integer NOT NULL,
    print_order_id bigint NOT NULL,
    print_price_id bigint NOT NULL
);


ALTER TABLE public.frames OWNER TO durachok;

--
-- Name: TABLE frames; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.frames IS 'Кадры';


--
-- Name: COLUMN frames.amount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.frames.amount IS 'Количество копий';


--
-- Name: COLUMN frames.print_order_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.frames.print_order_id IS 'Связь с заказом';


--
-- Name: frames_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.frames ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.frames_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: items; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.items (
    id bigint NOT NULL,
    product_name character varying(100) NOT NULL,
    price numeric(15,2) NOT NULL,
    firm_id bigint NOT NULL
);


ALTER TABLE public.items OWNER TO durachok;

--
-- Name: TABLE items; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.items IS 'Товары';


--
-- Name: COLUMN items.product_name; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.items.product_name IS 'Название товара';


--
-- Name: COLUMN items.price; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.items.price IS 'Цена';


--
-- Name: COLUMN items.firm_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.items.firm_id IS 'Связь с брэндом';


--
-- Name: items_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.items ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.items_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: kiosks; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.kiosks (
    outlet_id bigint NOT NULL,
    branch_outlet_id bigint NOT NULL
);


ALTER TABLE public.kiosks OWNER TO durachok;

--
-- Name: TABLE kiosks; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.kiosks IS 'Киоск';


--
-- Name: COLUMN kiosks.outlet_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.kiosks.outlet_id IS 'Здание где расположен';


--
-- Name: COLUMN kiosks.branch_outlet_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.kiosks.branch_outlet_id IS 'Киоск связан с филиалом';


--
-- Name: orders; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.orders (
    id bigint NOT NULL,
    accept_timestamp timestamp with time zone NOT NULL,
    total_amount_price numeric,
    is_urgent boolean NOT NULL,
    accept_outlet_id bigint NOT NULL,
    client_id bigint NOT NULL,
    CONSTRAINT orders_total_amount_price_check CHECK ((total_amount_price >= (0)::numeric))
);


ALTER TABLE public.orders OWNER TO durachok;

--
-- Name: TABLE orders; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.orders IS 'Заказы';


--
-- Name: COLUMN orders.accept_timestamp; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.orders.accept_timestamp IS 'Когда заказ поступил';


--
-- Name: COLUMN orders.total_amount_price; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.orders.total_amount_price IS 'Суммарная цена заказа, которая рассчитывается из купленных товаров, услуг....';


--
-- Name: COLUMN orders.is_urgent; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.orders.is_urgent IS 'Срочность заказа';


--
-- Name: COLUMN orders.accept_outlet_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.orders.accept_outlet_id IS 'Где приняли заказ';


--
-- Name: COLUMN orders.client_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.orders.client_id IS 'Клиент, который сделал заказ';


--
-- Name: orders_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.orders ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: outlet_types; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.outlet_types (
    id bigint NOT NULL,
    name character varying(200) NOT NULL
);


ALTER TABLE public.outlet_types OWNER TO durachok;

--
-- Name: TABLE outlet_types; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.outlet_types IS 'Типы зданий';


--
-- Name: outlet_types_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.outlet_types ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.outlet_types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: outlets; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.outlets (
    id bigint NOT NULL,
    address character varying(200) NOT NULL,
    num_workers integer NOT NULL,
    type_id bigint NOT NULL
);


ALTER TABLE public.outlets OWNER TO durachok;

--
-- Name: TABLE outlets; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.outlets IS 'Здания';


--
-- Name: COLUMN outlets.address; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.outlets.address IS 'Адрес';


--
-- Name: COLUMN outlets.type_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.outlets.type_id IS 'Тип здания';


--
-- Name: outlets_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.outlets ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.outlets_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: paper_sizes; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.paper_sizes (
    id bigint NOT NULL,
    name character varying(30) NOT NULL
);


ALTER TABLE public.paper_sizes OWNER TO durachok;

--
-- Name: TABLE paper_sizes; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.paper_sizes IS 'Формат бумаги';


--
-- Name: paper_sizes_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.paper_sizes ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.paper_sizes_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: paper_types; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.paper_types (
    id bigint NOT NULL,
    name character varying(30) NOT NULL
);


ALTER TABLE public.paper_types OWNER TO durachok;

--
-- Name: TABLE paper_types; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.paper_types IS 'Тип бумаги';


--
-- Name: paper_types_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.paper_types ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.paper_types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: photo_stores; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.photo_stores (
    outlet_id bigint NOT NULL
);


ALTER TABLE public.photo_stores OWNER TO durachok;

--
-- Name: TABLE photo_stores; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.photo_stores IS 'Фотомагазины';


--
-- Name: COLUMN photo_stores.outlet_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.photo_stores.outlet_id IS 'Привязка к зданию';


--
-- Name: print_discounts; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.print_discounts (
    id bigint NOT NULL,
    photo_amount integer NOT NULL,
    discount integer NOT NULL,
    CONSTRAINT print_discounts_discount_check CHECK ((discount >= 0))
);


ALTER TABLE public.print_discounts OWNER TO durachok;

--
-- Name: TABLE print_discounts; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.print_discounts IS 'Скидка на печать';


--
-- Name: COLUMN print_discounts.photo_amount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_discounts.photo_amount IS 'Количество фотографий, критерий';


--
-- Name: COLUMN print_discounts.discount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_discounts.discount IS 'Размер скидки в процентах';


--
-- Name: print_discounts_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.print_discounts ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.print_discounts_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: print_orders; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.print_orders (
    id bigint NOT NULL,
    discount_id bigint,
    order_id bigint NOT NULL
);


ALTER TABLE public.print_orders OWNER TO durachok;

--
-- Name: TABLE print_orders; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.print_orders IS 'Заказы на печать';


--
-- Name: COLUMN print_orders.discount_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_orders.discount_id IS 'Скидка';


--
-- Name: COLUMN print_orders.order_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_orders.order_id IS 'Связь с заказом';


--
-- Name: print_orders_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.print_orders ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.print_orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: print_prices; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.print_prices (
    id bigint NOT NULL,
    price smallint NOT NULL,
    paper_size_id bigint NOT NULL,
    paper_type_id bigint NOT NULL,
    CONSTRAINT print_prices_price_check CHECK ((price >= 0))
);


ALTER TABLE public.print_prices OWNER TO durachok;

--
-- Name: TABLE print_prices; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.print_prices IS 'Расценки на печать';


--
-- Name: COLUMN print_prices.price; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_prices.price IS 'Цена за (формат, печать)';


--
-- Name: COLUMN print_prices.paper_size_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_prices.paper_size_id IS 'Формат бумаги';


--
-- Name: COLUMN print_prices.paper_type_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.print_prices.paper_type_id IS 'Тип бумаги';


--
-- Name: print_prices_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.print_prices ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.print_prices_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: sale_orders; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.sale_orders (
    id bigint NOT NULL,
    amount smallint NOT NULL,
    item_id bigint NOT NULL,
    order_id bigint NOT NULL,
    CONSTRAINT sale_orders_amount_check CHECK ((amount >= 0))
);


ALTER TABLE public.sale_orders OWNER TO durachok;

--
-- Name: TABLE sale_orders; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.sale_orders IS 'Продажи товаров в заказе';


--
-- Name: COLUMN sale_orders.amount; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.sale_orders.amount IS 'Количество';


--
-- Name: COLUMN sale_orders.item_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.sale_orders.item_id IS 'Связь с товарами';


--
-- Name: COLUMN sale_orders.order_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.sale_orders.order_id IS 'Связь с заказом';


--
-- Name: sale_orders_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.sale_orders ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.sale_orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: service_orders; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.service_orders (
    id bigint NOT NULL,
    count integer NOT NULL,
    order_id bigint NOT NULL,
    service_type_id bigint NOT NULL
);


ALTER TABLE public.service_orders OWNER TO durachok;

--
-- Name: TABLE service_orders; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.service_orders IS 'Заказ услуг';


--
-- Name: COLUMN service_orders.count; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.service_orders.count IS 'Количество таких услуг заказанных';


--
-- Name: COLUMN service_orders.order_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.service_orders.order_id IS 'Связь с заказом';


--
-- Name: COLUMN service_orders.service_type_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.service_orders.service_type_id IS 'Связь с типом услуги';


--
-- Name: service_orders_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.service_orders ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.service_orders_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: service_types; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.service_types (
    id bigint NOT NULL,
    name character varying(100) NOT NULL,
    price numeric(15,2) NOT NULL
);


ALTER TABLE public.service_types OWNER TO durachok;

--
-- Name: service_types_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.service_types ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.service_types_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: service_types_needed_items; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.service_types_needed_items (
    item_id bigint NOT NULL,
    count integer NOT NULL,
    service_type_id bigint NOT NULL
);


ALTER TABLE public.service_types_needed_items OWNER TO durachok;

--
-- Name: service_types_outlets; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.service_types_outlets (
    id bigint NOT NULL,
    outlet_type_id bigint NOT NULL,
    service_type_id bigint NOT NULL
);


ALTER TABLE public.service_types_outlets OWNER TO durachok;

--
-- Name: TABLE service_types_outlets; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.service_types_outlets IS 'Типы услуг';


--
-- Name: service_types_outlets_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.service_types_outlets ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.service_types_outlets_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: storage; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.storage (
    id bigint NOT NULL,
    is_main_storage boolean,
    capacity integer NOT NULL,
    outlet_id bigint NOT NULL
);


ALTER TABLE public.storage OWNER TO durachok;

--
-- Name: TABLE storage; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.storage IS 'Склады зданий';


--
-- Name: storage_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.storage ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.storage_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: storage_items; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.storage_items (
    id bigint NOT NULL,
    quantity smallint NOT NULL,
    item_id bigint NOT NULL,
    storage_id bigint NOT NULL,
    CONSTRAINT storage_items_quantity_check CHECK ((quantity >= 0))
);


ALTER TABLE public.storage_items OWNER TO durachok;

--
-- Name: TABLE storage_items; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.storage_items IS 'Товары на складах зданий';


--
-- Name: storage_items_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.storage_items ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.storage_items_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: vendor_items; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.vendor_items (
    id bigint NOT NULL,
    current_price smallint NOT NULL,
    quantity smallint NOT NULL,
    item_id bigint NOT NULL,
    vendor_id bigint NOT NULL,
    CONSTRAINT vendor_items_current_price_check CHECK ((current_price >= 0)),
    CONSTRAINT vendor_items_quantity_check CHECK ((quantity >= 0))
);


ALTER TABLE public.vendor_items OWNER TO durachok;

--
-- Name: TABLE vendor_items; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.vendor_items IS 'По какой цене продает поставщик товары';


--
-- Name: COLUMN vendor_items.current_price; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.vendor_items.current_price IS 'Текущая цена по которой продает поставщик данный товар';


--
-- Name: COLUMN vendor_items.quantity; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.vendor_items.quantity IS 'Текущая цена по которой продает поставщик данный товар';


--
-- Name: COLUMN vendor_items.item_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.vendor_items.item_id IS 'Связь с товаром';


--
-- Name: COLUMN vendor_items.vendor_id; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.vendor_items.vendor_id IS 'Связь с поставщиком';


--
-- Name: vendor_items_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.vendor_items ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.vendor_items_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: vendors; Type: TABLE; Schema: public; Owner: durachok
--

CREATE TABLE public.vendors (
    id bigint NOT NULL,
    name character varying(100) NOT NULL
);


ALTER TABLE public.vendors OWNER TO durachok;

--
-- Name: TABLE vendors; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON TABLE public.vendors IS 'Поставщики';


--
-- Name: COLUMN vendors.name; Type: COMMENT; Schema: public; Owner: durachok
--

COMMENT ON COLUMN public.vendors.name IS 'Название компании (поставщика)';


--
-- Name: vendors_id_seq; Type: SEQUENCE; Schema: public; Owner: durachok
--

ALTER TABLE public.vendors ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.vendors_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Data for Name: auth_group; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.auth_group (id, name) FROM stdin;
1	Кассиры
2	Менеджеры по закупке
\.


--
-- Data for Name: auth_group_permissions; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.auth_group_permissions (id, group_id, permission_id) FROM stdin;
1	1	56
2	1	25
3	1	28
4	1	84
5	1	96
6	1	136
7	1	112
8	1	116
9	1	60
10	2	32
11	2	64
12	2	128
13	2	77
14	2	80
15	2	29
16	2	120
17	2	124
\.


--
-- Data for Name: auth_permission; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.auth_permission (id, name, content_type_id, codename) FROM stdin;
1	Can add log entry	1	add_logentry
2	Can change log entry	1	change_logentry
3	Can delete log entry	1	delete_logentry
4	Can view log entry	1	view_logentry
5	Can add permission	2	add_permission
6	Can change permission	2	change_permission
7	Can delete permission	2	delete_permission
8	Can view permission	2	view_permission
9	Can add group	3	add_group
10	Can change group	3	change_group
11	Can delete group	3	delete_group
12	Can view group	3	view_group
13	Can add content type	4	add_contenttype
14	Can change content type	4	change_contenttype
15	Can delete content type	4	delete_contenttype
16	Can view content type	4	view_contenttype
17	Can add session	5	add_session
18	Can change session	5	change_session
19	Can delete session	5	delete_session
20	Can view session	5	view_session
21	Can add Здание	6	add_outlet
22	Can change Здание	6	change_outlet
23	Can delete Здание	6	delete_outlet
24	Can view Здание	6	view_outlet
25	Can add Клиент(а)	7	add_client
26	Can change Клиент(а)	7	change_client
27	Can delete Клиент(а)	7	delete_client
28	Can view Клиент(а)	7	view_client
29	Can add Поставка	8	add_delivery
30	Can change Поставка	8	change_delivery
31	Can delete Поставка	8	delete_delivery
32	Can view Поставка	8	view_delivery
33	Can add Бренд	9	add_firm
34	Can change Бренд	9	change_firm
35	Can delete Бренд	9	delete_firm
36	Can view Бренд	9	view_firm
37	Can add Товар	10	add_item
38	Can change Товар	10	change_item
39	Can delete Товар	10	delete_item
40	Can view Товар	10	view_item
41	Can add Тип здания	11	add_outlettype
42	Can change Тип здания	11	change_outlettype
43	Can delete Тип здания	11	delete_outlettype
44	Can view Тип здания	11	view_outlettype
45	Can add Размер бумаги	12	add_papersize
46	Can change Размер бумаги	12	change_papersize
47	Can delete Размер бумаги	12	delete_papersize
48	Can view Размер бумаги	12	view_papersize
49	Can add Тип бумаги	13	add_papertype
50	Can change Тип бумаги	13	change_papertype
51	Can delete Тип бумаги	13	delete_papertype
52	Can view Тип бумаги	13	view_papertype
53	Can add Скидка на печать	14	add_printdiscount
54	Can change Скидка на печать	14	change_printdiscount
55	Can delete Скидка на печать	14	delete_printdiscount
56	Can view Скидка на печать	14	view_printdiscount
57	Can add Тип услуги	15	add_servicetype
58	Can change Тип услуги	15	change_servicetype
59	Can delete Тип услуги	15	delete_servicetype
60	Can view Тип услуги	15	view_servicetype
61	Can add Поставщик	16	add_vendor
62	Can change Поставщик	16	change_vendor
63	Can delete Поставщик	16	delete_vendor
64	Can view Поставщик	16	view_vendor
65	Can add Филиал	17	add_branch
66	Can change Филиал	17	change_branch
67	Can delete Филиал	17	delete_branch
68	Can view Филиал	17	view_branch
69	Can add Фотомагазин	18	add_photostore
70	Can change Фотомагазин	18	change_photostore
71	Can delete Фотомагазин	18	delete_photostore
72	Can view Фотомагазин	18	view_photostore
73	Can add Пользователь	19	add_customuser
74	Can change Пользователь	19	change_customuser
75	Can delete Пользователь	19	delete_customuser
76	Can view Пользователь	19	view_customuser
77	Can add Товар в поставке	20	add_deliveryitem
78	Can change Товар в поставке	20	change_deliveryitem
79	Can delete Товар в поставке	20	delete_deliveryitem
80	Can view Товар в поставке	20	view_deliveryitem
81	Can add Заказ	21	add_order
82	Can change Заказ	21	change_order
83	Can delete Заказ	21	delete_order
84	Can view Заказ	21	view_order
85	Can add Заказ на печать	22	add_printorder
86	Can change Заказ на печать	22	change_printorder
87	Can delete Заказ на печать	22	delete_printorder
88	Can view Заказ на печать	22	view_printorder
89	Can add Расценка на печать	23	add_printprice
90	Can change Расценка на печать	23	change_printprice
91	Can delete Расценка на печать	23	delete_printprice
92	Can view Расценка на печать	23	view_printprice
93	Can add Кадр	24	add_frame
94	Can change Кадр	24	change_frame
95	Can delete Кадр	24	delete_frame
96	Can view Кадр	24	view_frame
97	Can add Продажа товара в заказе	25	add_saleorder
98	Can change Продажа товара в заказе	25	change_saleorder
99	Can delete Продажа товара в заказе	25	delete_saleorder
100	Can view Продажа товара в заказе	25	view_saleorder
101	Can add Заказ услуги	26	add_serviceorder
102	Can change Заказ услуги	26	change_serviceorder
103	Can delete Заказ услуги	26	delete_serviceorder
104	Can view Заказ услуги	26	view_serviceorder
105	Can add Проявка пленок	27	add_filmdevelopmentorder
106	Can change Проявка пленок	27	change_filmdevelopmentorder
107	Can delete Проявка пленок	27	delete_filmdevelopmentorder
108	Can view Проявка пленок	27	view_filmdevelopmentorder
109	Can add Фотопленка	28	add_film
110	Can change Фотопленка	28	change_film
111	Can delete Фотопленка	28	delete_film
112	Can view Фотопленка	28	view_film
113	Can add Вид услуги в точке обслуживания	29	add_servicetypeoutlet
114	Can change Вид услуги в точке обслуживания	29	change_servicetypeoutlet
115	Can delete Вид услуги в точке обслуживания	29	delete_servicetypeoutlet
116	Can view Вид услуги в точке обслуживания	29	view_servicetypeoutlet
117	Can add Склад	30	add_storage
118	Can change Склад	30	change_storage
119	Can delete Склад	30	delete_storage
120	Can view Склад	30	view_storage
121	Can add Товар на складе	31	add_storageitem
122	Can change Товар на складе	31	change_storageitem
123	Can delete Товар на складе	31	delete_storageitem
124	Can view Товар на складе	31	view_storageitem
125	Can add Товар поставщика	32	add_vendoritem
126	Can change Товар поставщика	32	change_vendoritem
127	Can delete Товар поставщика	32	delete_vendoritem
128	Can view Товар поставщика	32	view_vendoritem
129	Can add Киоск	33	add_kiosk
130	Can change Киоск	33	change_kiosk
131	Can delete Киоск	33	delete_kiosk
132	Can view Киоск	33	view_kiosk
133	Can add Необходимые предметы для типа услуги	34	add_servicetypeneededitem
134	Can change Необходимые предметы для типа услуги	34	change_servicetypeneededitem
135	Can delete Необходимые предметы для типа услуги	34	delete_servicetypeneededitem
136	Can view Необходимые предметы для типа услуги	34	view_servicetypeneededitem
\.


--
-- Data for Name: branches; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.branches (outlet_id) FROM stdin;
6
7
\.


--
-- Data for Name: clients; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.clients (id, full_name, is_professional, discount) FROM stdin;
1	Иванов Иван Иванович	f	10
2	Петров Петр Петрович	t	20
3	Сидоров Сидор Сидорович	f	15
4	Кузнецова Елена Владимировна	f	0
5	Смирнова Ольга Петровна	t	25
6	Михайлов Александр Игоревич	f	5
7	Николаев Николай Николаевич	t	30
\.


--
-- Data for Name: deliveries; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.deliveries (id, delivery_date, storage_id, vendor_id) FROM stdin;
7	2024-05-31	1	1
9	2024-05-01	1	2
10	2024-05-01	1	3
\.


--
-- Data for Name: delivery_items; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.delivery_items (id, purchase_price, amount, delivery_id, item_id) FROM stdin;
2	10.00	100	7	5
3	50.00	150	9	3
4	100.00	100	9	4
5	10000.00	15	10	2
\.


--
-- Data for Name: django_admin_log; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.django_admin_log (id, action_time, object_id, object_repr, action_flag, change_message, content_type_id, user_id) FROM stdin;
1	2024-05-19 10:59:34.895155+00	1	durachok	2	[{"changed": {"fields": ["\\u0417\\u0434\\u0430\\u043d\\u0438\\u0435"]}}]	19	1
2	2024-05-19 18:50:38.773527+00	8	Иванов Иван Иванович - ул. Садовая, 987 -2024-05-19 18:37:26.524177+00:00	3		21	1
3	2024-05-19 20:11:06.072622+00	15	Фотомагазин - ул. Садовая, 987 Fujifilm - Фиксаж - 5	1	[{"added": {}}]	31	1
4	2024-05-19 20:11:29.137885+00	2	Фотомагазин - ул. Садовая, 987	2	[{"changed": {"fields": ["Capacity"]}}]	30	1
5	2024-05-19 20:11:34.51344+00	17	Фотомагазин - ул. Садовая, 987 Fujifilm - Очиститель - 5	1	[{"added": {}}]	31	1
6	2024-05-19 20:59:35.982552+00	9	Киоск - пер. Цветочный, 789 Fujifilm - Фиксаж - 10	2	[{"changed": {"fields": ["\\u041a\\u043e\\u043b\\u0438\\u0447\\u0435\\u0441\\u0442\\u0432\\u043e"]}}]	31	1
7	2024-05-19 21:01:24.731065+00	17	Фотомагазин - ул. Садовая, 987 Fujifilm - Очиститель - 7	2	[{"changed": {"fields": ["\\u041a\\u043e\\u043b\\u0438\\u0447\\u0435\\u0441\\u0442\\u0432\\u043e"]}}]	31	1
8	2024-05-19 21:01:47.770219+00	15	Фотомагазин - ул. Садовая, 987 Fujifilm - Фиксаж - 5	2	[{"changed": {"fields": ["\\u041a\\u043e\\u043b\\u0438\\u0447\\u0435\\u0441\\u0442\\u0432\\u043e"]}}]	31	1
9	2024-05-20 01:49:42.671441+00	1	Кассиры	1	[{"added": {}}]	3	1
10	2024-05-20 01:50:18.933299+00	2	cassir	1	[{"added": {}}]	19	1
11	2024-05-20 01:51:24.908998+00	2	cassir	3		19	1
12	2024-05-20 01:51:51.964069+00	3	cashier	1	[{"added": {}}]	19	1
13	2024-05-20 01:52:27.649796+00	3	cashier	2	[{"changed": {"fields": ["Staff status"]}}]	19	1
14	2024-05-20 01:53:22.314886+00	3	cashier	2	[{"changed": {"fields": ["Groups"]}}]	19	1
15	2024-05-20 03:11:27.828301+00	1	Кассиры	2	[{"changed": {"fields": ["Permissions"]}}]	3	1
16	2024-05-20 03:17:45.776934+00	1	durachok	2	[{"changed": {"fields": ["First name"]}}]	19	1
17	2024-05-20 03:17:52.931544+00	3	cashier	2	[{"changed": {"fields": ["First name"]}}]	19	1
18	2024-05-27 02:16:58.248515+00	2	Менеджер по закупке	1	[{"added": {}}]	3	1
19	2024-05-27 02:17:16.121875+00	2	Менеджеры по закупке	2	[{"changed": {"fields": ["Name"]}}]	3	1
20	2024-05-27 02:25:05.512393+00	4	manager	1	[{"added": {}}]	19	1
21	2024-05-27 02:25:16.01438+00	4	manager	2	[{"changed": {"fields": ["Groups"]}}]	19	1
22	2024-05-27 02:25:26.440481+00	4	manager	2	[{"changed": {"fields": ["First name"]}}]	19	1
23	2024-05-27 02:25:36.776044+00	3	cashier	2	[{"changed": {"fields": ["First name"]}}]	19	1
24	2024-05-27 02:25:45.724391+00	1	durachok	2	[{"changed": {"fields": ["First name"]}}]	19	1
25	2024-05-27 02:26:31.032319+00	4	manager	2	[{"changed": {"fields": ["Staff status"]}}]	19	1
26	2024-05-27 02:34:42.557491+00	2	Менеджеры по закупке	2	[{"changed": {"fields": ["Permissions"]}}]	3	1
\.


--
-- Data for Name: django_content_type; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.django_content_type (id, app_label, model) FROM stdin;
1	admin	logentry
2	auth	permission
3	auth	group
4	contenttypes	contenttype
5	sessions	session
6	entities	outlet
7	entities	client
8	entities	delivery
9	entities	firm
10	entities	item
11	entities	outlettype
12	entities	papersize
13	entities	papertype
14	entities	printdiscount
15	entities	servicetype
16	entities	vendor
17	entities	branch
18	entities	photostore
19	entities	customuser
20	entities	deliveryitem
21	entities	order
22	entities	printorder
23	entities	printprice
24	entities	frame
25	entities	saleorder
26	entities	serviceorder
27	entities	filmdevelopmentorder
28	entities	film
29	entities	servicetypeoutlet
30	entities	storage
31	entities	storageitem
32	entities	vendoritem
33	entities	kiosk
34	entities	servicetypeneededitem
\.


--
-- Data for Name: django_migrations; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.django_migrations (id, app, name, applied) FROM stdin;
1	contenttypes	0001_initial	2024-05-19 07:34:50.352685+00
2	contenttypes	0002_remove_content_type_name	2024-05-19 07:34:50.356816+00
3	auth	0001_initial	2024-05-19 07:34:50.380331+00
4	auth	0002_alter_permission_name_max_length	2024-05-19 07:34:50.384592+00
5	auth	0003_alter_user_email_max_length	2024-05-19 07:34:50.387887+00
6	auth	0004_alter_user_username_opts	2024-05-19 07:34:50.391184+00
7	auth	0005_alter_user_last_login_null	2024-05-19 07:34:50.395537+00
8	auth	0006_require_contenttypes_0002	2024-05-19 07:34:50.396663+00
9	auth	0007_alter_validators_add_error_messages	2024-05-19 07:34:50.400298+00
10	auth	0008_alter_user_username_max_length	2024-05-19 07:34:50.403954+00
11	auth	0009_alter_user_last_name_max_length	2024-05-19 07:34:50.407605+00
12	auth	0010_alter_group_name_max_length	2024-05-19 07:34:50.411555+00
13	auth	0011_update_proxy_permissions	2024-05-19 07:34:50.415345+00
14	auth	0012_alter_user_first_name_max_length	2024-05-19 07:34:50.41913+00
15	entities	0001_initial	2024-05-19 07:34:50.814232+00
16	admin	0001_initial	2024-05-19 07:34:50.838657+00
17	admin	0002_logentry_remove_auto_add	2024-05-19 07:34:50.84538+00
18	admin	0003_logentry_add_action_flag_choices	2024-05-19 07:34:50.851687+00
19	sessions	0001_initial	2024-05-19 07:34:50.859634+00
\.


--
-- Data for Name: django_session; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.django_session (session_key, session_data, expire_date) FROM stdin;
qp67tz1fykzey39aimshndu4td7f71cs	.eJxVjMEOwiAQRP-FsyEIW1o8evcbyLK7laqBpLQn47_bJj3oHOe9mbeKuC45rk3mOLG6KFCn3y4hPaXsgB9Y7lVTLcs8Jb0r-qBN3yrL63q4fwcZW97WduyNMKMwcEJnO9MF64A8QyAYhLZ42_NAQNJJ7-05jN4xDTCaZJz6fAEJeziW:1sBQtB:zKmxiPfyYoegjPfyq6MJIC6Joygm08dxd08JTZ1DlpY	2024-06-10 03:19:09.748981+00
\.


--
-- Data for Name: entities_customuser; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.entities_customuser (id, password, last_login, is_superuser, username, first_name, last_name, email, is_staff, is_active, date_joined, outlet_id) FROM stdin;
1	pbkdf2_sha256$720000$5i2e7AMkTJcOiED2H3yfYy$h5oZIzDhdMq0dJRXpwdCx0I5uAySAhQDrhowAsGcevE=	2024-05-27 03:01:17.361069+00	t	durachok	Админ		fenixrnd@mail.ru	t	t	2024-05-19 07:44:05+00	1
3	pbkdf2_sha256$720000$VNwCa3OwqvgyTofpjT0ew9$07XBMfgL2LSMOaZ8E7Zj+8loDD5FRifqprIzdItohEA=	2024-05-27 03:18:55.841855+00	f	cashier	Кассир			t	t	2024-05-20 01:51:51+00	1
4	pbkdf2_sha256$720000$ZH0NrNAS9s6yWq3kOiMaV1$Ixg6WuOCHFbzu4Cj6o0U2at+xb3pE70ICg975KQrYos=	2024-05-27 03:19:09.745725+00	f	manager	Менеджер по закупке			t	t	2024-05-27 02:25:05+00	1
\.


--
-- Data for Name: entities_customuser_groups; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.entities_customuser_groups (id, customuser_id, group_id) FROM stdin;
1	3	1
2	4	2
\.


--
-- Data for Name: entities_customuser_user_permissions; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.entities_customuser_user_permissions (id, customuser_id, permission_id) FROM stdin;
\.


--
-- Data for Name: film_development_orders; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.film_development_orders (id, code, service_order_id) FROM stdin;
4	EE5678	2
\.


--
-- Data for Name: films; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.films (id, code, service_order_id) FROM stdin;
1	CAF	1
9	CAF1	20
11	CAF12	22
12	CAF3	27
23	CAF5	42
\.


--
-- Data for Name: firms; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.firms (id, name) FROM stdin;
1	Canon
2	Nikon
3	Sony
4	Fujifilm
5	Panasonic
6	Olympus
7	Samsung
8	Pentax
9	Leica
10	Hasselblad
\.


--
-- Data for Name: frames; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.frames (id, amount, code, frame_number, print_order_id, print_price_id) FROM stdin;
1	1	CAF	2	1	1
7	1	269e64dd-8fc2-4396-80ce-9b803ba07e7c	1	9	1
8	1	4b8538f1-b7fd-43ee-9731-c786dbf04b79	1	10	1
\.


--
-- Data for Name: items; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.items (id, product_name, price, firm_id) FROM stdin;
1	Canon EOS 5D Mark IV	3500.00	1
2	Nikon D850	3200.00	2
3	Фиксаж	50.00	4
4	Очиститель	75.00	4
6	Sony Alpha a7 III	2500.01	3
5	Пленка простая	100.00	5
\.


--
-- Data for Name: kiosks; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.kiosks (outlet_id, branch_outlet_id) FROM stdin;
2	6
4	6
5	7
\.


--
-- Data for Name: orders; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.orders (id, accept_timestamp, total_amount_price, is_urgent, accept_outlet_id, client_id) FROM stdin;
2	2024-04-23 12:36:42+00	450	f	1	1
4	2024-05-01 12:37:27+00	43	f	4	3
5	2024-04-30 15:00:00+00	213	t	6	3
7	2024-04-18 06:04:47+00	432	f	2	7
6	2024-05-01 06:00:00+00	3150	f	4	1
3	2024-04-27 09:00:00+00	3951	f	2	2
37	2024-05-19 21:01:51.828317+00	14103	f	1	1
95	2024-05-21 10:40:00.084406+00	143	t	1	6
93	2024-05-21 10:33:10.6394+00	100	f	1	2
97	2024-05-27 02:55:56.789981+00	500	f	1	4
49	2024-05-19 21:55:06.483256+00	14310	f	1	1
50	2024-05-19 21:56:08.665249+00	14310	f	1	1
51	2024-05-20 02:26:25.80346+00	14324	f	1	1
52	2024-05-20 02:26:37.931307+00	4082	f	1	1
30	2024-05-19 20:55:52.177021+00	873	f	1	1
32	2024-05-19 20:56:15.592585+00	3753	f	1	1
\.


--
-- Data for Name: outlet_types; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.outlet_types (id, name) FROM stdin;
1	Филиал
2	Киоск
3	Фотомагазин
4	Главный склад
\.


--
-- Data for Name: outlets; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.outlets (id, address, num_workers, type_id) FROM stdin;
1	ул. Садовая, 987	4	3
2	пер. Цветочный, 789	2	2
3	Ул Садовая 17	2	4
4	ул. Зеленая, 321	2	2
5	просп. Лесной, 654	1	2
6	ул. Ленина, дом 123	5	1
7	пр. Победы, 456	3	1
8	ул. Чехова 19	2	3
\.


--
-- Data for Name: paper_sizes; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.paper_sizes (id, name) FROM stdin;
1	A0
2	A1
3	A2
4	A3
5	A4
6	A6
7	A7
8	A8
9	A9
10	A5
\.


--
-- Data for Name: paper_types; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.paper_types (id, name) FROM stdin;
1	Матовая
2	Глянцевая
3	Холст
4	Атласная
5	Полуглянцевая
\.


--
-- Data for Name: photo_stores; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.photo_stores (outlet_id) FROM stdin;
1
8
\.


--
-- Data for Name: print_discounts; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.print_discounts (id, photo_amount, discount) FROM stdin;
1	5	10
2	10	20
3	30	50
4	100	70
\.


--
-- Data for Name: print_orders; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.print_orders (id, discount_id, order_id) FROM stdin;
1	2	3
2	2	7
9	\N	51
10	\N	52
\.


--
-- Data for Name: print_prices; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.print_prices (id, price, paper_size_id, paper_type_id) FROM stdin;
1	15	1	1
2	12	2	2
3	10	3	3
4	20	4	4
\.


--
-- Data for Name: sale_orders; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.sale_orders (id, amount, item_id, order_id) FROM stdin;
2	1	1	6
3	1	1	3
7	1	4	93
8	1	3	93
11	1	4	95
\.


--
-- Data for Name: service_orders; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.service_orders (id, count, order_id, service_type_id) FROM stdin;
1	1	3	2
2	1	3	3
3	1	5	1
4	2	3	1
5	2	7	1
20	1	30	3
22	1	32	3
27	1	37	3
39	1	49	1
40	1	50	1
41	1	51	1
42	1	52	3
80	2	97	1
\.


--
-- Data for Name: service_types; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.service_types (id, name, price) FROM stdin;
1	Фото на паспорт	250.00
2	Ретушь	500.00
3	Проявка плёнки	20.00
4	Скан документа	50.01
\.


--
-- Data for Name: service_types_needed_items; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.service_types_needed_items (item_id, count, service_type_id) FROM stdin;
3	2	3
4	1	3
\.


--
-- Data for Name: service_types_outlets; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.service_types_outlets (id, outlet_type_id, service_type_id) FROM stdin;
2	3	2
3	1	4
4	2	4
5	3	4
6	1	3
7	3	3
1	3	1
\.


--
-- Data for Name: storage; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.storage (id, is_main_storage, capacity, outlet_id) FROM stdin;
1	t	300	3
4	f	15	4
5	f	12	5
6	f	8	6
7	f	37	7
8	f	19	8
3	f	30	2
2	f	18	1
\.


--
-- Data for Name: storage_items; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.storage_items (id, quantity, item_id, storage_id) FROM stdin;
1	5	2	2
2	10	2	8
5	5	5	4
7	8	5	6
8	5	5	7
3	5	5	3
12	3	1	4
13	1	6	3
14	0	1	3
9	6	3	3
11	5	4	3
15	0	3	2
17	3	4	2
\.


--
-- Data for Name: vendor_items; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.vendor_items (id, current_price, quantity, item_id, vendor_id) FROM stdin;
5	50	2	4	2
6	75	1	3	2
4	2500	3	6	3
3	100	1	5	1
2	3200	2	2	3
1	3500	1	1	3
\.


--
-- Data for Name: vendors; Type: TABLE DATA; Schema: public; Owner: durachok
--

COPY public.vendors (id, name) FROM stdin;
1	Поставщик Пленок
2	Поставщик реактивов
3	Поставщик фотоаппаратов
\.


--
-- Name: auth_group_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.auth_group_id_seq', 2, true);


--
-- Name: auth_group_permissions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.auth_group_permissions_id_seq', 17, true);


--
-- Name: auth_permission_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.auth_permission_id_seq', 136, true);


--
-- Name: clients_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.clients_id_seq', 7, true);


--
-- Name: deliveries_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.deliveries_id_seq', 10, true);


--
-- Name: delivery_items_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.delivery_items_id_seq', 5, true);


--
-- Name: django_admin_log_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.django_admin_log_id_seq', 26, true);


--
-- Name: django_content_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.django_content_type_id_seq', 34, true);


--
-- Name: django_migrations_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.django_migrations_id_seq', 19, true);


--
-- Name: entities_customuser_groups_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.entities_customuser_groups_id_seq', 2, true);


--
-- Name: entities_customuser_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.entities_customuser_id_seq', 4, true);


--
-- Name: entities_customuser_user_permissions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.entities_customuser_user_permissions_id_seq', 1, false);


--
-- Name: film_development_orders_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.film_development_orders_id_seq', 4, true);


--
-- Name: films_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.films_id_seq', 32, true);


--
-- Name: firms_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.firms_id_seq', 10, true);


--
-- Name: frames_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.frames_id_seq', 8, true);


--
-- Name: items_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.items_id_seq', 6, true);


--
-- Name: orders_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.orders_id_seq', 97, true);


--
-- Name: outlet_types_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.outlet_types_id_seq', 4, true);


--
-- Name: outlets_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.outlets_id_seq', 8, true);


--
-- Name: paper_sizes_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.paper_sizes_id_seq', 10, true);


--
-- Name: paper_types_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.paper_types_id_seq', 5, true);


--
-- Name: print_discounts_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.print_discounts_id_seq', 4, true);


--
-- Name: print_orders_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.print_orders_id_seq', 10, true);


--
-- Name: print_prices_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.print_prices_id_seq', 4, true);


--
-- Name: sale_orders_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.sale_orders_id_seq', 11, true);


--
-- Name: service_orders_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.service_orders_id_seq', 80, true);


--
-- Name: service_types_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.service_types_id_seq', 4, true);


--
-- Name: service_types_outlets_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.service_types_outlets_id_seq', 7, true);


--
-- Name: storage_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.storage_id_seq', 8, true);


--
-- Name: storage_items_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.storage_items_id_seq', 17, true);


--
-- Name: vendor_items_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.vendor_items_id_seq', 6, true);


--
-- Name: vendors_id_seq; Type: SEQUENCE SET; Schema: public; Owner: durachok
--

SELECT pg_catalog.setval('public.vendors_id_seq', 3, true);


--
-- Name: auth_group auth_group_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group
    ADD CONSTRAINT auth_group_name_key UNIQUE (name);


--
-- Name: auth_group_permissions auth_group_permissions_group_id_permission_id_0cd325b0_uniq; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_group_id_permission_id_0cd325b0_uniq UNIQUE (group_id, permission_id);


--
-- Name: auth_group_permissions auth_group_permissions_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_pkey PRIMARY KEY (id);


--
-- Name: auth_group auth_group_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group
    ADD CONSTRAINT auth_group_pkey PRIMARY KEY (id);


--
-- Name: auth_permission auth_permission_content_type_id_codename_01ab375a_uniq; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_permission
    ADD CONSTRAINT auth_permission_content_type_id_codename_01ab375a_uniq UNIQUE (content_type_id, codename);


--
-- Name: auth_permission auth_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_permission
    ADD CONSTRAINT auth_permission_pkey PRIMARY KEY (id);


--
-- Name: branches branches_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_pkey PRIMARY KEY (outlet_id);


--
-- Name: clients clients_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.clients
    ADD CONSTRAINT clients_pkey PRIMARY KEY (id);


--
-- Name: deliveries deliveries_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.deliveries
    ADD CONSTRAINT deliveries_pkey PRIMARY KEY (id);


--
-- Name: delivery_items delivery_items_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.delivery_items
    ADD CONSTRAINT delivery_items_pkey PRIMARY KEY (id);


--
-- Name: delivery_items delivery_items_unique; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.delivery_items
    ADD CONSTRAINT delivery_items_unique UNIQUE (delivery_id, item_id);


--
-- Name: django_admin_log django_admin_log_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_admin_log
    ADD CONSTRAINT django_admin_log_pkey PRIMARY KEY (id);


--
-- Name: django_content_type django_content_type_app_label_model_76bd3d3b_uniq; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_content_type
    ADD CONSTRAINT django_content_type_app_label_model_76bd3d3b_uniq UNIQUE (app_label, model);


--
-- Name: django_content_type django_content_type_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_content_type
    ADD CONSTRAINT django_content_type_pkey PRIMARY KEY (id);


--
-- Name: django_migrations django_migrations_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_migrations
    ADD CONSTRAINT django_migrations_pkey PRIMARY KEY (id);


--
-- Name: django_session django_session_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_session
    ADD CONSTRAINT django_session_pkey PRIMARY KEY (session_key);


--
-- Name: entities_customuser_groups entities_customuser_groups_customuser_id_group_id_cb8768a9_uniq; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_groups
    ADD CONSTRAINT entities_customuser_groups_customuser_id_group_id_cb8768a9_uniq UNIQUE (customuser_id, group_id);


--
-- Name: entities_customuser_groups entities_customuser_groups_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_groups
    ADD CONSTRAINT entities_customuser_groups_pkey PRIMARY KEY (id);


--
-- Name: entities_customuser entities_customuser_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser
    ADD CONSTRAINT entities_customuser_pkey PRIMARY KEY (id);


--
-- Name: entities_customuser_user_permissions entities_customuser_user_customuser_id_permission_69f8f5c8_uniq; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_user_permissions
    ADD CONSTRAINT entities_customuser_user_customuser_id_permission_69f8f5c8_uniq UNIQUE (customuser_id, permission_id);


--
-- Name: entities_customuser_user_permissions entities_customuser_user_permissions_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_user_permissions
    ADD CONSTRAINT entities_customuser_user_permissions_pkey PRIMARY KEY (id);


--
-- Name: entities_customuser entities_customuser_username_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser
    ADD CONSTRAINT entities_customuser_username_key UNIQUE (username);


--
-- Name: film_development_orders film_development_orders_code_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.film_development_orders
    ADD CONSTRAINT film_development_orders_code_key UNIQUE (code);


--
-- Name: film_development_orders film_development_orders_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.film_development_orders
    ADD CONSTRAINT film_development_orders_pkey PRIMARY KEY (id);


--
-- Name: films films_code_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.films
    ADD CONSTRAINT films_code_key UNIQUE (code);


--
-- Name: films films_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.films
    ADD CONSTRAINT films_pkey PRIMARY KEY (id);


--
-- Name: firms firms_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.firms
    ADD CONSTRAINT firms_name_key UNIQUE (name);


--
-- Name: firms firms_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.firms
    ADD CONSTRAINT firms_pkey PRIMARY KEY (id);


--
-- Name: frames frames_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.frames
    ADD CONSTRAINT frames_pkey PRIMARY KEY (id);


--
-- Name: items items_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.items
    ADD CONSTRAINT items_pkey PRIMARY KEY (id);


--
-- Name: kiosks kiosks_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.kiosks
    ADD CONSTRAINT kiosks_pkey PRIMARY KEY (outlet_id);


--
-- Name: orders orders_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_pkey PRIMARY KEY (id);


--
-- Name: outlet_types outlet_types_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.outlet_types
    ADD CONSTRAINT outlet_types_name_key UNIQUE (name);


--
-- Name: outlet_types outlet_types_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.outlet_types
    ADD CONSTRAINT outlet_types_pkey PRIMARY KEY (id);


--
-- Name: outlets outlets_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.outlets
    ADD CONSTRAINT outlets_pkey PRIMARY KEY (id);


--
-- Name: paper_sizes paper_sizes_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.paper_sizes
    ADD CONSTRAINT paper_sizes_name_key UNIQUE (name);


--
-- Name: paper_sizes paper_sizes_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.paper_sizes
    ADD CONSTRAINT paper_sizes_pkey PRIMARY KEY (id);


--
-- Name: paper_types paper_types_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.paper_types
    ADD CONSTRAINT paper_types_name_key UNIQUE (name);


--
-- Name: paper_types paper_types_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.paper_types
    ADD CONSTRAINT paper_types_pkey PRIMARY KEY (id);


--
-- Name: photo_stores photo_stores_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.photo_stores
    ADD CONSTRAINT photo_stores_pkey PRIMARY KEY (outlet_id);


--
-- Name: print_discounts print_discounts_photo_amount_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_discounts
    ADD CONSTRAINT print_discounts_photo_amount_key UNIQUE (photo_amount);


--
-- Name: print_discounts print_discounts_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_discounts
    ADD CONSTRAINT print_discounts_pkey PRIMARY KEY (id);


--
-- Name: print_orders print_orders_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_orders
    ADD CONSTRAINT print_orders_pkey PRIMARY KEY (id);


--
-- Name: print_prices print_prices_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_prices
    ADD CONSTRAINT print_prices_pkey PRIMARY KEY (id);


--
-- Name: sale_orders sale_orders_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.sale_orders
    ADD CONSTRAINT sale_orders_pkey PRIMARY KEY (id);


--
-- Name: service_orders service_orders_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_orders
    ADD CONSTRAINT service_orders_pkey PRIMARY KEY (id);


--
-- Name: service_types_needed_items service_types_needed_items_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_needed_items
    ADD CONSTRAINT service_types_needed_items_pkey PRIMARY KEY (item_id);


--
-- Name: service_types_outlets service_types_outlets_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_outlets
    ADD CONSTRAINT service_types_outlets_pkey PRIMARY KEY (id);


--
-- Name: service_types service_types_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types
    ADD CONSTRAINT service_types_pkey PRIMARY KEY (id);


--
-- Name: storage_items storage_items_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage_items
    ADD CONSTRAINT storage_items_pkey PRIMARY KEY (id);


--
-- Name: storage_items storage_items_unique; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage_items
    ADD CONSTRAINT storage_items_unique UNIQUE (storage_id, item_id);


--
-- Name: storage storage_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage
    ADD CONSTRAINT storage_pkey PRIMARY KEY (id);


--
-- Name: film_development_orders unique_film_development_order; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.film_development_orders
    ADD CONSTRAINT unique_film_development_order UNIQUE (service_order_id, code);


--
-- Name: items unique_item_firm_product_name; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.items
    ADD CONSTRAINT unique_item_firm_product_name UNIQUE (firm_id, product_name);


--
-- Name: print_prices unique_print_price; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_prices
    ADD CONSTRAINT unique_print_price UNIQUE (paper_size_id, paper_type_id);


--
-- Name: sale_orders unique_sale_order_item; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.sale_orders
    ADD CONSTRAINT unique_sale_order_item UNIQUE (order_id, item_id);


--
-- Name: service_orders unique_service_order; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_orders
    ADD CONSTRAINT unique_service_order UNIQUE (service_type_id, order_id);


--
-- Name: service_types_needed_items unique_service_type_needed_item; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_needed_items
    ADD CONSTRAINT unique_service_type_needed_item UNIQUE (item_id, service_type_id);


--
-- Name: outlets unique_type_address; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.outlets
    ADD CONSTRAINT unique_type_address UNIQUE (type_id, address);


--
-- Name: vendor_items vendor_items_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendor_items
    ADD CONSTRAINT vendor_items_pkey PRIMARY KEY (id);


--
-- Name: vendor_items vendor_items_unique; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendor_items
    ADD CONSTRAINT vendor_items_unique UNIQUE (vendor_id, item_id);


--
-- Name: vendors vendors_name_key; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendors
    ADD CONSTRAINT vendors_name_key UNIQUE (name);


--
-- Name: vendors vendors_pkey; Type: CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendors
    ADD CONSTRAINT vendors_pkey PRIMARY KEY (id);


--
-- Name: auth_group_name_a6ea08ec_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX auth_group_name_a6ea08ec_like ON public.auth_group USING btree (name varchar_pattern_ops);


--
-- Name: auth_group_permissions_group_id_b120cbf9; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX auth_group_permissions_group_id_b120cbf9 ON public.auth_group_permissions USING btree (group_id);


--
-- Name: auth_group_permissions_permission_id_84c5c92e; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX auth_group_permissions_permission_id_84c5c92e ON public.auth_group_permissions USING btree (permission_id);


--
-- Name: auth_permission_content_type_id_2f476e4b; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX auth_permission_content_type_id_2f476e4b ON public.auth_permission USING btree (content_type_id);


--
-- Name: deliveries_storage_id_838117ae; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX deliveries_storage_id_838117ae ON public.deliveries USING btree (storage_id);


--
-- Name: deliveries_vendor_id_0fea7d79; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX deliveries_vendor_id_0fea7d79 ON public.deliveries USING btree (vendor_id);


--
-- Name: delivery_items_delivery_id_14393145; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX delivery_items_delivery_id_14393145 ON public.delivery_items USING btree (delivery_id);


--
-- Name: delivery_items_item_id_789a9378; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX delivery_items_item_id_789a9378 ON public.delivery_items USING btree (item_id);


--
-- Name: django_admin_log_content_type_id_c4bce8eb; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX django_admin_log_content_type_id_c4bce8eb ON public.django_admin_log USING btree (content_type_id);


--
-- Name: django_admin_log_user_id_c564eba6; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX django_admin_log_user_id_c564eba6 ON public.django_admin_log USING btree (user_id);


--
-- Name: django_session_expire_date_a5c62663; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX django_session_expire_date_a5c62663 ON public.django_session USING btree (expire_date);


--
-- Name: django_session_session_key_c0390e0f_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX django_session_session_key_c0390e0f_like ON public.django_session USING btree (session_key varchar_pattern_ops);


--
-- Name: entities_customuser_groups_customuser_id_a7615322; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_groups_customuser_id_a7615322 ON public.entities_customuser_groups USING btree (customuser_id);


--
-- Name: entities_customuser_groups_group_id_1e8d65e2; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_groups_group_id_1e8d65e2 ON public.entities_customuser_groups USING btree (group_id);


--
-- Name: entities_customuser_outlet_id_c005bbc7; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_outlet_id_c005bbc7 ON public.entities_customuser USING btree (outlet_id);


--
-- Name: entities_customuser_user_permissions_customuser_id_3912157e; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_user_permissions_customuser_id_3912157e ON public.entities_customuser_user_permissions USING btree (customuser_id);


--
-- Name: entities_customuser_user_permissions_permission_id_54c0dc1e; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_user_permissions_permission_id_54c0dc1e ON public.entities_customuser_user_permissions USING btree (permission_id);


--
-- Name: entities_customuser_username_5ec1d632_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX entities_customuser_username_5ec1d632_like ON public.entities_customuser USING btree (username varchar_pattern_ops);


--
-- Name: film_development_orders_code_de9d8ab0_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX film_development_orders_code_de9d8ab0_like ON public.film_development_orders USING btree (code varchar_pattern_ops);


--
-- Name: film_development_orders_service_order_id_b9ff7e0a; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX film_development_orders_service_order_id_b9ff7e0a ON public.film_development_orders USING btree (service_order_id);


--
-- Name: films_code_47632e31_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX films_code_47632e31_like ON public.films USING btree (code varchar_pattern_ops);


--
-- Name: films_service_order_id_06f106dd; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX films_service_order_id_06f106dd ON public.films USING btree (service_order_id);


--
-- Name: firms_name_a8b8cb4a_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX firms_name_a8b8cb4a_like ON public.firms USING btree (name varchar_pattern_ops);


--
-- Name: frames_print_order_id_95ce7291; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX frames_print_order_id_95ce7291 ON public.frames USING btree (print_order_id);


--
-- Name: frames_print_price_id_a1e7d84f; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX frames_print_price_id_a1e7d84f ON public.frames USING btree (print_price_id);


--
-- Name: items_firm_id_851bf272; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX items_firm_id_851bf272 ON public.items USING btree (firm_id);


--
-- Name: kiosks_branch_outlet_id_8e3388dd; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX kiosks_branch_outlet_id_8e3388dd ON public.kiosks USING btree (branch_outlet_id);


--
-- Name: orders_accept_outlet_id_467fc260; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX orders_accept_outlet_id_467fc260 ON public.orders USING btree (accept_outlet_id);


--
-- Name: orders_client_id_67f0b211; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX orders_client_id_67f0b211 ON public.orders USING btree (client_id);


--
-- Name: outlet_types_name_16529543_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX outlet_types_name_16529543_like ON public.outlet_types USING btree (name varchar_pattern_ops);


--
-- Name: outlets_type_id_da874c02; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX outlets_type_id_da874c02 ON public.outlets USING btree (type_id);


--
-- Name: paper_sizes_name_803def33_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX paper_sizes_name_803def33_like ON public.paper_sizes USING btree (name varchar_pattern_ops);


--
-- Name: paper_types_name_8beeead5_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX paper_types_name_8beeead5_like ON public.paper_types USING btree (name varchar_pattern_ops);


--
-- Name: print_orders_discount_id_9fd8a759; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX print_orders_discount_id_9fd8a759 ON public.print_orders USING btree (discount_id);


--
-- Name: print_orders_order_id_e86b48f3; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX print_orders_order_id_e86b48f3 ON public.print_orders USING btree (order_id);


--
-- Name: print_prices_paper_size_id_b6bd91de; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX print_prices_paper_size_id_b6bd91de ON public.print_prices USING btree (paper_size_id);


--
-- Name: print_prices_paper_type_id_d25ca5c9; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX print_prices_paper_type_id_d25ca5c9 ON public.print_prices USING btree (paper_type_id);


--
-- Name: sale_orders_item_id_1681178a; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX sale_orders_item_id_1681178a ON public.sale_orders USING btree (item_id);


--
-- Name: sale_orders_order_id_5694b1b0; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX sale_orders_order_id_5694b1b0 ON public.sale_orders USING btree (order_id);


--
-- Name: service_orders_order_id_856e764e; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX service_orders_order_id_856e764e ON public.service_orders USING btree (order_id);


--
-- Name: service_orders_service_type_id_c217911c; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX service_orders_service_type_id_c217911c ON public.service_orders USING btree (service_type_id);


--
-- Name: service_types_needed_items_service_type_id_ef628a82; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX service_types_needed_items_service_type_id_ef628a82 ON public.service_types_needed_items USING btree (service_type_id);


--
-- Name: service_types_outlets_outlet_type_id_89acc1f8; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX service_types_outlets_outlet_type_id_89acc1f8 ON public.service_types_outlets USING btree (outlet_type_id);


--
-- Name: service_types_outlets_service_type_id_ebf300ea; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX service_types_outlets_service_type_id_ebf300ea ON public.service_types_outlets USING btree (service_type_id);


--
-- Name: storage_items_item_id_b78b3109; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX storage_items_item_id_b78b3109 ON public.storage_items USING btree (item_id);


--
-- Name: storage_items_storage_id_03df6bc4; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX storage_items_storage_id_03df6bc4 ON public.storage_items USING btree (storage_id);


--
-- Name: storage_outlet_id_ad437302; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX storage_outlet_id_ad437302 ON public.storage USING btree (outlet_id);


--
-- Name: vendor_items_item_id_2a54402d; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX vendor_items_item_id_2a54402d ON public.vendor_items USING btree (item_id);


--
-- Name: vendor_items_vendor_id_011850d5; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX vendor_items_vendor_id_011850d5 ON public.vendor_items USING btree (vendor_id);


--
-- Name: vendors_name_3cb15b1c_like; Type: INDEX; Schema: public; Owner: durachok
--

CREATE INDEX vendors_name_3cb15b1c_like ON public.vendors USING btree (name varchar_pattern_ops);


--
-- Name: sale_orders check_and_update_storage_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_and_update_storage_trigger BEFORE INSERT ON public.sale_orders FOR EACH ROW EXECUTE FUNCTION public.check_and_update_storage();


--
-- Name: branches check_branch_outlet_type_consistency; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_branch_outlet_type_consistency BEFORE INSERT ON public.branches FOR EACH ROW EXECUTE FUNCTION public.check_outlet_type_consistency('Филиал');


--
-- Name: film_development_orders check_correct_service_type_selected_film_order; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_correct_service_type_selected_film_order BEFORE INSERT OR UPDATE ON public.film_development_orders FOR EACH ROW EXECUTE FUNCTION public.check_film_development_order();


--
-- Name: kiosks check_kiosk_outlet_type_consistency; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_kiosk_outlet_type_consistency BEFORE INSERT ON public.kiosks FOR EACH ROW EXECUTE FUNCTION public.check_outlet_type_consistency('Киоск');


--
-- Name: deliveries check_main_storage_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_main_storage_trigger BEFORE INSERT ON public.deliveries FOR EACH ROW EXECUTE FUNCTION public.check_main_storage();


--
-- Name: service_orders check_material_availability_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_material_availability_trigger BEFORE INSERT ON public.service_orders FOR EACH ROW EXECUTE FUNCTION public.check_material_availability();


--
-- Name: service_orders check_outlet_service_type_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_outlet_service_type_trigger BEFORE INSERT OR UPDATE ON public.service_orders FOR EACH ROW EXECUTE FUNCTION public.check_outlet_service_type();


--
-- Name: photo_stores check_photo_store_outlet_type_consistency; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_photo_store_outlet_type_consistency BEFORE INSERT ON public.photo_stores FOR EACH ROW EXECUTE FUNCTION public.check_outlet_type_consistency('Фотомагазин');


--
-- Name: storage_items check_storage_capacity_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_storage_capacity_trigger BEFORE INSERT OR UPDATE ON public.storage_items FOR EACH ROW EXECUTE FUNCTION public.check_storage_capacity();


--
-- Name: delivery_items check_vendor_item_trigger; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER check_vendor_item_trigger BEFORE INSERT ON public.delivery_items FOR EACH ROW EXECUTE FUNCTION public.check_vendor_item_exists();


--
-- Name: branches delete_outlet_from_branches; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER delete_outlet_from_branches AFTER DELETE ON public.branches FOR EACH ROW EXECUTE FUNCTION public.delete_outlet_on_junction_delete();


--
-- Name: kiosks delete_outlet_from_kiosks; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER delete_outlet_from_kiosks AFTER DELETE ON public.kiosks FOR EACH ROW EXECUTE FUNCTION public.delete_outlet_on_junction_delete();


--
-- Name: photo_stores delete_outlet_from_photo_stores; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER delete_outlet_from_photo_stores AFTER DELETE ON public.photo_stores FOR EACH ROW EXECUTE FUNCTION public.delete_outlet_on_junction_delete();


--
-- Name: frames print_order_discount_check; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER print_order_discount_check AFTER INSERT OR UPDATE ON public.frames FOR EACH ROW EXECUTE FUNCTION public.auto_set_print_discount();


--
-- Name: sale_orders update_price_trigger_for_orders; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER update_price_trigger_for_orders AFTER INSERT OR DELETE OR UPDATE ON public.sale_orders FOR EACH ROW EXECUTE FUNCTION public.update_price_trigger();


--
-- Name: print_orders update_price_trigger_for_print_orders; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER update_price_trigger_for_print_orders AFTER INSERT OR DELETE OR UPDATE ON public.print_orders FOR EACH ROW EXECUTE FUNCTION public.update_price_trigger();


--
-- Name: service_orders update_price_trigger_for_service_orders; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER update_price_trigger_for_service_orders AFTER INSERT OR DELETE OR UPDATE ON public.service_orders FOR EACH ROW EXECUTE FUNCTION public.update_price_trigger();


--
-- Name: orders urgent_order_type_check; Type: TRIGGER; Schema: public; Owner: durachok
--

CREATE TRIGGER urgent_order_type_check BEFORE INSERT ON public.orders FOR EACH ROW EXECUTE FUNCTION public.check_urgent_order_type();


--
-- Name: auth_group_permissions auth_group_permissio_permission_id_84c5c92e_fk_auth_perm; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group_permissions
    ADD CONSTRAINT auth_group_permissio_permission_id_84c5c92e_fk_auth_perm FOREIGN KEY (permission_id) REFERENCES public.auth_permission(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: auth_group_permissions auth_group_permissions_group_id_b120cbf9_fk_auth_group_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_group_id_b120cbf9_fk_auth_group_id FOREIGN KEY (group_id) REFERENCES public.auth_group(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: auth_permission auth_permission_content_type_id_2f476e4b_fk_django_co; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.auth_permission
    ADD CONSTRAINT auth_permission_content_type_id_2f476e4b_fk_django_co FOREIGN KEY (content_type_id) REFERENCES public.django_content_type(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: branches branches_outlet_id_a53f29c1_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.branches
    ADD CONSTRAINT branches_outlet_id_a53f29c1_fk_outlets_id FOREIGN KEY (outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: deliveries deliveries_storage_id_838117ae_fk_storage_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.deliveries
    ADD CONSTRAINT deliveries_storage_id_838117ae_fk_storage_id FOREIGN KEY (storage_id) REFERENCES public.storage(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: deliveries deliveries_vendor_id_0fea7d79_fk_vendors_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.deliveries
    ADD CONSTRAINT deliveries_vendor_id_0fea7d79_fk_vendors_id FOREIGN KEY (vendor_id) REFERENCES public.vendors(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: delivery_items delivery_items_delivery_id_14393145_fk_deliveries_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.delivery_items
    ADD CONSTRAINT delivery_items_delivery_id_14393145_fk_deliveries_id FOREIGN KEY (delivery_id) REFERENCES public.deliveries(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: delivery_items delivery_items_item_id_789a9378_fk_items_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.delivery_items
    ADD CONSTRAINT delivery_items_item_id_789a9378_fk_items_id FOREIGN KEY (item_id) REFERENCES public.items(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: django_admin_log django_admin_log_content_type_id_c4bce8eb_fk_django_co; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_admin_log
    ADD CONSTRAINT django_admin_log_content_type_id_c4bce8eb_fk_django_co FOREIGN KEY (content_type_id) REFERENCES public.django_content_type(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: django_admin_log django_admin_log_user_id_c564eba6_fk_entities_customuser_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.django_admin_log
    ADD CONSTRAINT django_admin_log_user_id_c564eba6_fk_entities_customuser_id FOREIGN KEY (user_id) REFERENCES public.entities_customuser(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: entities_customuser_user_permissions entities_customuser__customuser_id_3912157e_fk_entities_; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_user_permissions
    ADD CONSTRAINT entities_customuser__customuser_id_3912157e_fk_entities_ FOREIGN KEY (customuser_id) REFERENCES public.entities_customuser(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: entities_customuser_groups entities_customuser__customuser_id_a7615322_fk_entities_; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_groups
    ADD CONSTRAINT entities_customuser__customuser_id_a7615322_fk_entities_ FOREIGN KEY (customuser_id) REFERENCES public.entities_customuser(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: entities_customuser_user_permissions entities_customuser__permission_id_54c0dc1e_fk_auth_perm; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_user_permissions
    ADD CONSTRAINT entities_customuser__permission_id_54c0dc1e_fk_auth_perm FOREIGN KEY (permission_id) REFERENCES public.auth_permission(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: entities_customuser_groups entities_customuser_groups_group_id_1e8d65e2_fk_auth_group_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser_groups
    ADD CONSTRAINT entities_customuser_groups_group_id_1e8d65e2_fk_auth_group_id FOREIGN KEY (group_id) REFERENCES public.auth_group(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: entities_customuser entities_customuser_outlet_id_c005bbc7_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.entities_customuser
    ADD CONSTRAINT entities_customuser_outlet_id_c005bbc7_fk_outlets_id FOREIGN KEY (outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: film_development_orders film_development_ord_service_order_id_b9ff7e0a_fk_service_o; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.film_development_orders
    ADD CONSTRAINT film_development_ord_service_order_id_b9ff7e0a_fk_service_o FOREIGN KEY (service_order_id) REFERENCES public.service_orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: films films_service_order_id_06f106dd_fk_service_orders_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.films
    ADD CONSTRAINT films_service_order_id_06f106dd_fk_service_orders_id FOREIGN KEY (service_order_id) REFERENCES public.service_orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: frames frames_print_order_id_95ce7291_fk_print_orders_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.frames
    ADD CONSTRAINT frames_print_order_id_95ce7291_fk_print_orders_id FOREIGN KEY (print_order_id) REFERENCES public.print_orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: frames frames_print_price_id_a1e7d84f_fk_print_prices_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.frames
    ADD CONSTRAINT frames_print_price_id_a1e7d84f_fk_print_prices_id FOREIGN KEY (print_price_id) REFERENCES public.print_prices(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: items items_firm_id_851bf272_fk_firms_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.items
    ADD CONSTRAINT items_firm_id_851bf272_fk_firms_id FOREIGN KEY (firm_id) REFERENCES public.firms(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: kiosks kiosks_branch_outlet_id_8e3388dd_fk_branches_outlet_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.kiosks
    ADD CONSTRAINT kiosks_branch_outlet_id_8e3388dd_fk_branches_outlet_id FOREIGN KEY (branch_outlet_id) REFERENCES public.branches(outlet_id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: kiosks kiosks_outlet_id_a5d45315_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.kiosks
    ADD CONSTRAINT kiosks_outlet_id_a5d45315_fk_outlets_id FOREIGN KEY (outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: orders orders_accept_outlet_id_467fc260_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_accept_outlet_id_467fc260_fk_outlets_id FOREIGN KEY (accept_outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: orders orders_client_id_67f0b211_fk_clients_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.orders
    ADD CONSTRAINT orders_client_id_67f0b211_fk_clients_id FOREIGN KEY (client_id) REFERENCES public.clients(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: outlets outlets_type_id_da874c02_fk_outlet_types_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.outlets
    ADD CONSTRAINT outlets_type_id_da874c02_fk_outlet_types_id FOREIGN KEY (type_id) REFERENCES public.outlet_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: photo_stores photo_stores_outlet_id_a95ad198_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.photo_stores
    ADD CONSTRAINT photo_stores_outlet_id_a95ad198_fk_outlets_id FOREIGN KEY (outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: print_orders print_orders_discount_id_9fd8a759_fk_print_discounts_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_orders
    ADD CONSTRAINT print_orders_discount_id_9fd8a759_fk_print_discounts_id FOREIGN KEY (discount_id) REFERENCES public.print_discounts(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: print_orders print_orders_order_id_e86b48f3_fk_orders_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_orders
    ADD CONSTRAINT print_orders_order_id_e86b48f3_fk_orders_id FOREIGN KEY (order_id) REFERENCES public.orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: print_prices print_prices_paper_size_id_b6bd91de_fk_paper_sizes_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_prices
    ADD CONSTRAINT print_prices_paper_size_id_b6bd91de_fk_paper_sizes_id FOREIGN KEY (paper_size_id) REFERENCES public.paper_sizes(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: print_prices print_prices_paper_type_id_d25ca5c9_fk_paper_types_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.print_prices
    ADD CONSTRAINT print_prices_paper_type_id_d25ca5c9_fk_paper_types_id FOREIGN KEY (paper_type_id) REFERENCES public.paper_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: sale_orders sale_orders_item_id_1681178a_fk_items_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.sale_orders
    ADD CONSTRAINT sale_orders_item_id_1681178a_fk_items_id FOREIGN KEY (item_id) REFERENCES public.items(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: sale_orders sale_orders_order_id_5694b1b0_fk_orders_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.sale_orders
    ADD CONSTRAINT sale_orders_order_id_5694b1b0_fk_orders_id FOREIGN KEY (order_id) REFERENCES public.orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_orders service_orders_order_id_856e764e_fk_orders_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_orders
    ADD CONSTRAINT service_orders_order_id_856e764e_fk_orders_id FOREIGN KEY (order_id) REFERENCES public.orders(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_orders service_orders_service_type_id_c217911c_fk_service_types_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_orders
    ADD CONSTRAINT service_orders_service_type_id_c217911c_fk_service_types_id FOREIGN KEY (service_type_id) REFERENCES public.service_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_types_needed_items service_types_needed_items_item_id_afaf4eb1_fk_items_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_needed_items
    ADD CONSTRAINT service_types_needed_items_item_id_afaf4eb1_fk_items_id FOREIGN KEY (item_id) REFERENCES public.items(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_types_needed_items service_types_needed_service_type_id_ef628a82_fk_service_t; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_needed_items
    ADD CONSTRAINT service_types_needed_service_type_id_ef628a82_fk_service_t FOREIGN KEY (service_type_id) REFERENCES public.service_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_types_outlets service_types_outlet_outlet_type_id_89acc1f8_fk_outlet_ty; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_outlets
    ADD CONSTRAINT service_types_outlet_outlet_type_id_89acc1f8_fk_outlet_ty FOREIGN KEY (outlet_type_id) REFERENCES public.outlet_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: service_types_outlets service_types_outlet_service_type_id_ebf300ea_fk_service_t; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.service_types_outlets
    ADD CONSTRAINT service_types_outlet_service_type_id_ebf300ea_fk_service_t FOREIGN KEY (service_type_id) REFERENCES public.service_types(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: storage_items storage_items_item_id_b78b3109_fk_items_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage_items
    ADD CONSTRAINT storage_items_item_id_b78b3109_fk_items_id FOREIGN KEY (item_id) REFERENCES public.items(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: storage_items storage_items_storage_id_03df6bc4_fk_storage_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage_items
    ADD CONSTRAINT storage_items_storage_id_03df6bc4_fk_storage_id FOREIGN KEY (storage_id) REFERENCES public.storage(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: storage storage_outlet_id_ad437302_fk_outlets_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.storage
    ADD CONSTRAINT storage_outlet_id_ad437302_fk_outlets_id FOREIGN KEY (outlet_id) REFERENCES public.outlets(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: vendor_items vendor_items_item_id_2a54402d_fk_items_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendor_items
    ADD CONSTRAINT vendor_items_item_id_2a54402d_fk_items_id FOREIGN KEY (item_id) REFERENCES public.items(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: vendor_items vendor_items_vendor_id_011850d5_fk_vendors_id; Type: FK CONSTRAINT; Schema: public; Owner: durachok
--

ALTER TABLE ONLY public.vendor_items
    ADD CONSTRAINT vendor_items_vendor_id_011850d5_fk_vendors_id FOREIGN KEY (vendor_id) REFERENCES public.vendors(id) DEFERRABLE INITIALLY DEFERRED;


--
-- PostgreSQL database dump complete
--

