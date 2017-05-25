-- ======================================================================
-- File:     qpfdb.sql
--           QPFDB PostgreSQL Database creation script
--
-- Version:  1.2
--
-- Date:     2016/12/21
--
-- Author:   J C Gonzalez
--
-- Copyright (C) 2015,2016,2017 Euclid SOC Team @ ESAC
-- ======================================================================

SET statement_timeout = 0;
-- SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

-- ----------------------------------------------------------------------
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner:
CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;

-- ----------------------------------------------------------------------
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner:
COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

-- ======================================================================
-- TABLE: alerts
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: alerts; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TYPE alert_group     AS ENUM ('System', 'Diagnostics');
CREATE TYPE alert_severity  AS ENUM ('Warning', 'Error', 'Fatal');
CREATE TYPE alert_type      AS ENUM ('Resource', 'OutOfRange', 'Diagnostic');
CREATE TYPE alert_vartype   AS ENUM ('INT', 'FLOAT', 'DOUBLE');
CREATE TYPE alert_varvalue  AS (i integer, f float, d double precision);
CREATE TYPE alert_variable  AS (
    name   text,
    x      alert_varvalue,
    ll     alert_varvalue,
    ul     alert_varvalue,
    vtype  alert_vartype
);

CREATE TABLE alerts (
    alert_id   integer NOT NULL,
    creation   timestamp without time zone,
    grp        alert_group,
    sev        alert_severity,
    typ        alert_type,
    origin     text,
    msgs       text,
    var        alert_variable
);

ALTER TABLE alerts OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: alerts_alert_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE alerts_alert_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE alerts_alert_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: alerts_alert_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE alerts_alert_id_seq OWNED BY alerts.alert_id;

-- ======================================================================
-- TABLE: qpfstates
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: qpfstates; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TYPE qpf_state AS ENUM ('ERROR', 'OFF', 'INITIALISED', 'RUNNING', 'OPERATIONAL');

CREATE TABLE qpfstates (
    qpfstate_id   integer NOT NULL,
    timestmp      timestamp without time zone,
    sessionname   text,
    nodename      text,
    state         qpf_state
);

ALTER TABLE qpfstates OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: qpfstates_qpfstate_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE qpfstates_qpfstate_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE qpfstates_qpfstate_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: qpfstates_qpfstate_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE qpfstates_qpfstate_id_seq OWNED BY qpfstates.qpfstate_id;

-- ======================================================================
-- TABLE: configuration
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: configuration; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE configuration (
    created timestamp without time zone NOT NULL,
    cfg json,
    last_accessed timestamp without time zone
);

ALTER TABLE configuration OWNER TO eucops;

-- ======================================================================
-- TABLE: creators
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: creators; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE creators (
    creator_id integer NOT NULL,
    creator_desc character varying(128)
);

ALTER TABLE creators OWNER TO eucops;

-- ======================================================================
-- TABLE: instruments
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: instruments; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE instruments (
    instrument_id integer NOT NULL,
    instrument character varying(128)
);

ALTER TABLE instruments OWNER TO eucops;

-- ======================================================================
-- TABLE: message_type
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: message_type; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE message_type (
    message_type integer NOT NULL,
    type_desc character varying(128)
);

ALTER TABLE message_type OWNER TO eucops;

-- ======================================================================
-- TABLE: messages
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: messages; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE messages (
    id integer NOT NULL,
    message_id character varying(128) NOT NULL,
    message_type character varying(128) NOT NULL,
    origin character varying(128) NOT NULL,
    destination character varying(128) NOT NULL,
    message_version integer,
    creation_time timestamp without time zone,
    transmission_time timestamp without time zone,
    reception_time timestamp without time zone
);

ALTER TABLE messages OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: messages_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE messages_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE messages_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: messages_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE messages_id_seq OWNED BY messages.id;

-- ======================================================================
-- TABLE: node_states
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: node_states; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE node_states (
    node_name character varying(20) NOT NULL,
    node_state character varying(32)
);

ALTER TABLE node_states OWNER TO eucops;

-- ======================================================================
-- TABLE: observation_modes
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: observation_modes; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE observation_modes (
    obsmode_id integer NOT NULL,
    obsmode_desc character varying(128)
);

ALTER TABLE observation_modes OWNER TO eucops;

-- ======================================================================
-- TABLE: product_status
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: product_status; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE product_status (
    product_status_id integer NOT NULL,
    status_desc character varying(128)
);

ALTER TABLE product_status OWNER TO eucops;

-- ======================================================================
-- TABLE: products_info
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: products_info; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE products_info (
    id integer NOT NULL,
    product_id character varying(256) NOT NULL,
    product_type character varying(128),
    product_version character varying(128),
    product_size bigint,
    product_status_id integer,
    creator_id integer,
    instrument_id integer,
    start_time timestamp without time zone,
    end_time timestamp without time zone,
    registration_time timestamp without time zone,
    url character varying(1024),
    signature character varying,
    obsmode_id integer NOT NULL
);

ALTER TABLE products_info OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: products_info_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE products_info_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE products_info_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: products_info_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE products_info_id_seq OWNED BY products_info.id;

-- ======================================================================
-- TABLE: task_inputs
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: task_inputs; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE task_inputs (
    task_id character varying(128) NOT NULL,
    product_id character varying(256) NOT NULL
);

ALTER TABLE task_inputs OWNER TO eucops;

-- ======================================================================
-- TABLE: task_outputs
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: task_outputs; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE task_outputs (
    task_id character varying(128) NOT NULL,
    product_id character varying(256) NOT NULL
);

ALTER TABLE task_outputs OWNER TO eucops;

-- ======================================================================
-- TABLE: task_status
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: task_status; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE task_status (
    task_status_id integer NOT NULL,
    status_desc character varying(128)
);

ALTER TABLE task_status OWNER TO eucops;

-- ======================================================================
-- TABLE: tasks_info
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: tasks_info; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE tasks_info (
    id integer NOT NULL,
    task_id character varying(128) NOT NULL,
    task_status_id integer,
    task_exitcode integer,
    task_path character varying(1024),
    task_size integer,
    registration_time timestamp without time zone,
    start_time timestamp without time zone,
    end_time timestamp without time zone,
    task_data json
);

ALTER TABLE tasks_info OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: tasks_info_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE tasks_info_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE tasks_info_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: tasks_info_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE tasks_info_id_seq OWNED BY tasks_info.id;

-- ======================================================================
-- TABLE: transmissions
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: transmissions; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE transmissions (
    id integer NOT NULL,
    msg_date timestamp without time zone,
    msg_from character(64),
    msg_to character(64),
    msg_type character(64),
    msg_bcast character(1),
    msg_content text
);

ALTER TABLE transmissions OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: transmissions_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE transmissions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;

ALTER TABLE transmissions_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: transmissions_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE transmissions_id_seq OWNED BY transmissions.id;

-- ======================================================================
-- TABLE: icommands
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: icommands; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE icommands (
id integer NOT NULL,
cmd_date timestamp without time zone,
cmd_source character varying(32),
cmd_target character varying(32),
cmd_executed boolean,
cmd_content text
);

ALTER TABLE icommands OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: icommands_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE icommands_id_seq
START WITH 1
INCREMENT BY 1
NO MINVALUE
NO MAXVALUE
CACHE 1;

ALTER TABLE icommands_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: icommands_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE icommands_id_seq OWNED BY icommands.id;

-- ======================================================================
-- TABLE: pvc
-- ======================================================================

-- ----------------------------------------------------------------------
-- Name: pvc; Type: TABLE; Schema: public; Owner: eucops; Tablespace:
CREATE TABLE pvc (
id integer NOT NULL,
name character varying(32),
version character varying(32),
date timestamp without time zone,
counter integer,
comment character varying(128)
);

ALTER TABLE pvc OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: pvc_id_seq; Type: SEQUENCE; Schema: public; Owner: eucops
CREATE SEQUENCE pvc_id_seq
START WITH 1
INCREMENT BY 1
NO MINVALUE
NO MAXVALUE
CACHE 1;

ALTER TABLE pvc_id_seq OWNER TO eucops;

-- ----------------------------------------------------------------------
-- Name: pvc_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: eucops
ALTER SEQUENCE pvc_id_seq OWNED BY pvc.id;

-- ======================================================================
COPY pvc  (id, date, counter, version, name, comment) FROM stdin;
1	2016-02-09 14:01:42	1	V0.1                            	LE1_Processor                   	First version of LE1_processor script, just for testing purposes
2	2016-02-09 14:01:42	1	V1.0                            	QLA_Processor                   	First version of QLA_processor script, just for testing purposes
3	2017-01-09 14:01:42	2	V1.1                            	QLA_Processor                   	First ammend to version 1.0 of QLA_processor script, again just for testing purposes
4	2017-01-09 14:01:42	1	V0.1                            	Archive_Ingestor                   	First version of the Archive Ingestor script, just for testing purposes
\.

-- ----------------------------------------------------------------------
-- Name: alert_id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY alerts ALTER COLUMN alert_id SET DEFAULT nextval('alerts_alert_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: qpfstate_id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY qpfstates ALTER COLUMN qpfstate_id SET DEFAULT nextval('qpfstates_qpfstate_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY messages ALTER COLUMN id SET DEFAULT nextval('messages_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY products_info ALTER COLUMN id SET DEFAULT nextval('products_info_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY tasks_info ALTER COLUMN id SET DEFAULT nextval('tasks_info_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY transmissions ALTER COLUMN id SET DEFAULT nextval('transmissions_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: id; Type: DEFAULT; Schema: public; Owner: eucops
ALTER TABLE ONLY icommands ALTER COLUMN id SET DEFAULT nextval('icommands_id_seq'::regclass);

-- ----------------------------------------------------------------------
-- Name: alerts_alert_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('alerts_alert_id_seq', 1, false);

-- ----------------------------------------------------------------------
-- Name: qpfstates_qpfstate_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('qpfstates_qpfstate_id_seq', 1, false);


-- ----------------------------------------------------------------------
 -- Data for Name: creators; Type: TABLE DATA; Schema: public; Owner: eucops
COPY creators (creator_id, creator_desc) FROM stdin;
1	SOC_LE1
10	SOC_QLA.TEST
11	SOC_QLA.OPE
\.

-- ----------------------------------------------------------------------
 -- Data for Name: instruments; Type: TABLE DATA; Schema: public; Owner: eucops
COPY instruments (instrument_id, instrument) FROM stdin;
1	VIS
2	NIR
3	SIR
-1	UNKNOWN_INST
\.

-- ----------------------------------------------------------------------
 -- Data for Name: message_type; Type: TABLE DATA; Schema: public; Owner: eucops
COPY message_type (message_type, type_desc) FROM stdin;
\.

-- ----------------------------------------------------------------------
 -- Data for Name: messages; Type: TABLE DATA; Schema: public; Owner: eucops
COPY messages (id, message_id, message_type, origin, destination, message_version, creation_time, transmission_time, reception_time) FROM stdin;
\.

-- ----------------------------------------------------------------------
-- Name: messages_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('messages_id_seq', 1, false);

-- ----------------------------------------------------------------------
 -- Data for Name: node_states; Type: TABLE DATA; Schema: public; Owner: eucops
COPY node_states (node_name, node_state) FROM stdin;
\.

-- ----------------------------------------------------------------------
 -- Data for Name: observation_modes; Type: TABLE DATA; Schema: public; Owner: eucops
COPY observation_modes (obsmode_id, obsmode_desc) FROM stdin;
1	NOMINAL
2	TEST
\.

-- ----------------------------------------------------------------------
 -- Data for Name: product_status; Type: TABLE DATA; Schema: public; Owner: eucops
COPY product_status (product_status_id, status_desc) FROM stdin;
0	OK
1	NOTOK
\.

-- ----------------------------------------------------------------------
 -- Data for Name: products_info; Type: TABLE DATA; Schema: public; Owner: eucops
COPY products_info (id, product_id, product_type, product_version, product_size, product_status_id, creator_id, instrument_id, start_time, end_time, registration_time, url, signature, obsmode_id) FROM stdin;
\.

-- ----------------------------------------------------------------------
-- Name: products_info_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('products_info_id_seq', 69106, true);

-- ----------------------------------------------------------------------
 -- Data for Name: task_inputs; Type: TABLE DATA; Schema: public; Owner: eucops
COPY task_inputs (task_id, product_id) FROM stdin;
\.

-- ----------------------------------------------------------------------
 -- Data for Name: task_outputs; Type: TABLE DATA; Schema: public; Owner: eucops
COPY task_outputs (task_id, product_id) FROM stdin;
\.

-- ----------------------------------------------------------------------
 -- Data for Name: task_status; Type: TABLE DATA; Schema: public; Owner: eucops
COPY task_status (task_status_id, status_desc) FROM stdin;
-1	FAILED
0	FINISHED
1	RUNNING
2	WAITING
-2	SCHEDULED
\.

-- ----------------------------------------------------------------------
 -- Data for Name: tasks_info; Type: TABLE DATA; Schema: public; Owner: eucops
COPY tasks_info (id, task_id, task_status_id, task_exitcode, task_path, task_size, registration_time, start_time, end_time, task_data) FROM stdin;
\.

-- ----------------------------------------------------------------------
-- Name: tasks_info_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('tasks_info_id_seq', 22606, true);

-- ----------------------------------------------------------------------
 -- Data for Name: transmissions; Type: TABLE DATA; Schema: public; Owner: eucops
COPY transmissions (id, msg_date, msg_from, msg_to, msg_type, msg_bcast, msg_content) FROM stdin;
\.

-- ----------------------------------------------------------------------
-- Name: transmissions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: eucops
SELECT pg_catalog.setval('transmissions_id_seq', 928641, true);

-- ----------------------------------------------------------------------
-- Name: alerts_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY alerts
    ADD CONSTRAINT alerts_pkey
    PRIMARY KEY (alert_id);

-- ----------------------------------------------------------------------
-- Name: qpfstates_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY qpfstates
ADD CONSTRAINT qpfstates_pkey
    PRIMARY KEY (qpfstate_id);

-- ----------------------------------------------------------------------
-- Name: configuration_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY configuration
    ADD CONSTRAINT configuration_pkey
    PRIMARY KEY (created);

-- ----------------------------------------------------------------------
-- Name: creators_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY creators
    ADD CONSTRAINT creators_pkey
    PRIMARY KEY (creator_id);

-- ----------------------------------------------------------------------
-- Name: instruments_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY instruments
    ADD CONSTRAINT instruments_pkey
    PRIMARY KEY (instrument_id);

-- ----------------------------------------------------------------------
-- Name: message_type_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY message_type
    ADD CONSTRAINT message_type_pkey
    PRIMARY KEY (message_type);

-- ----------------------------------------------------------------------
-- Name: messages_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY messages
    ADD CONSTRAINT messages_pkey
    PRIMARY KEY (id, message_id, message_type, origin, destination);

-- ----------------------------------------------------------------------
-- Name: node_states_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY node_states
    ADD CONSTRAINT node_states_pkey
    PRIMARY KEY (node_name);

-- ----------------------------------------------------------------------
-- Name: observation_modes_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY observation_modes
    ADD CONSTRAINT observation_modes_pkey
    PRIMARY KEY (obsmode_id);

-- ----------------------------------------------------------------------
-- Name: product_status_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY product_status
    ADD CONSTRAINT product_status_pkey
    PRIMARY KEY (product_status_id);

-- ----------------------------------------------------------------------
-- Name: products_info_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY products_info
    ADD CONSTRAINT products_info_pkey
    PRIMARY KEY (product_id);

-- ----------------------------------------------------------------------
-- Name: task_inputs_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY task_inputs
    ADD CONSTRAINT task_inputs_pkey
    PRIMARY KEY (task_id, product_id);

-- ----------------------------------------------------------------------
-- Name: task_outputs_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY task_outputs
    ADD CONSTRAINT task_outputs_pkey
    PRIMARY KEY (task_id, product_id);

-- ----------------------------------------------------------------------
-- Name: task_status_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY task_status
    ADD CONSTRAINT task_status_pkey
    PRIMARY KEY (task_status_id);

-- ----------------------------------------------------------------------
-- Name: tasks_info_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY tasks_info
    ADD CONSTRAINT tasks_info_pkey
    PRIMARY KEY (task_id);

-- ----------------------------------------------------------------------
-- Name: transmissions_pkey; Type: CONSTRAINT; Schema: public; Owner: eucops; Tablespace:
ALTER TABLE ONLY transmissions
    ADD CONSTRAINT transmissions_pkey
    PRIMARY KEY (id);

-- ----------------------------------------------------------------------
-- Name: products_info_creator_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY products_info
    ADD CONSTRAINT products_info_creator_id_fkey
    FOREIGN KEY (creator_id)
    REFERENCES creators(creator_id);

-- ----------------------------------------------------------------------
-- Name: products_info_instrument_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY products_info
    ADD CONSTRAINT products_info_instrument_id_fkey
    FOREIGN KEY (instrument_id)
    REFERENCES instruments(instrument_id);

-- ----------------------------------------------------------------------
-- Name: products_info_product_status_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY products_info
    ADD CONSTRAINT products_info_product_status_id_fkey
    FOREIGN KEY (product_status_id)
    REFERENCES product_status(product_status_id);

-- ----------------------------------------------------------------------
-- Name: task_inputs_task_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY task_inputs
    ADD CONSTRAINT task_inputs_task_id_fkey
    FOREIGN KEY (task_id)
    REFERENCES tasks_info(task_id);

-- ----------------------------------------------------------------------
-- Name: task_outputs_task_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY task_outputs
    ADD CONSTRAINT task_outputs_task_id_fkey
    FOREIGN KEY (task_id)
    REFERENCES tasks_info(task_id);

-- ----------------------------------------------------------------------
-- Name: tasks_info_task_status_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: eucops
ALTER TABLE ONLY tasks_info
    ADD CONSTRAINT tasks_info_task_status_id_fkey
    FOREIGN KEY (task_status_id)
    REFERENCES task_status(task_status_id);

-- ----------------------------------------------------------------------
-- Name: public; Type: ACL; Schema: -; Owner: eucops
GRANT ALL ON SCHEMA public TO PUBLIC;
GRANT ALL ON SCHEMA public TO eucops;

-- EOF
