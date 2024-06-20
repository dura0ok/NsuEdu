CREATE OR REPLACE FUNCTION delete_outlet_on_junction_delete() RETURNS TRIGGER AS
$$
BEGIN
    IF OLD.outlet_id IS NOT NULL THEN
        DELETE FROM outlets WHERE id = OLD.outlet_id;
    END IF;
    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE TRIGGER delete_outlet_from_kiosks
    AFTER DELETE
    ON kiosks
    FOR EACH ROW
EXECUTE FUNCTION delete_outlet_on_junction_delete();

CREATE OR REPLACE TRIGGER delete_outlet_from_branches
    AFTER DELETE
    ON branches
    FOR EACH ROW
EXECUTE FUNCTION delete_outlet_on_junction_delete();

CREATE OR REPLACE TRIGGER delete_outlet_from_photo_stores
    AFTER DELETE
    ON photo_stores
    FOR EACH ROW
EXECUTE FUNCTION delete_outlet_on_junction_delete();
