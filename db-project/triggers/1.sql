--- Проверять при вставке нового киоска, филиала, фотомагазина,
--  что здание(outlet) имеет такой же тип(outlet_type_id)
CREATE OR REPLACE FUNCTION check_outlet_type_consistency() RETURNS TRIGGER AS
$$
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
$$ LANGUAGE plpgsql;

-- Триггер для киосков
CREATE OR REPLACE TRIGGER check_kiosk_outlet_type_consistency
    BEFORE INSERT
    ON kiosks
    FOR EACH ROW
EXECUTE FUNCTION check_outlet_type_consistency('Киоск');

-- Триггер для филиалов
CREATE OR REPLACE TRIGGER check_branch_outlet_type_consistency
    BEFORE INSERT
    ON branches
    FOR EACH ROW
EXECUTE FUNCTION check_outlet_type_consistency('Филиал');

-- Триггер для фотомагазинов
CREATE OR REPLACE TRIGGER check_photo_store_outlet_type_consistency
    BEFORE INSERT
    ON photo_stores
    FOR EACH ROW
EXECUTE FUNCTION check_outlet_type_consistency('Фотомагазин');
