-- Проверять что поставки(deliviries) могут осуществляться только в главный склад
CREATE OR REPLACE FUNCTION check_main_storage()
    RETURNS TRIGGER AS
$$
BEGIN
    IF NOT EXISTS (SELECT 1 FROM storage WHERE id = NEW.storage_id AND is_main_storage = TRUE) THEN
        RAISE EXCEPTION 'The storage specified is not marked as main storage.';
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER check_main_storage_trigger
    BEFORE INSERT
    ON deliveries
    FOR EACH ROW
EXECUTE FUNCTION check_main_storage();
