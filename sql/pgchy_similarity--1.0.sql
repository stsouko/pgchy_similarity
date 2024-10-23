-- pgchy_similarity--1.0.sql


CREATE FUNCTION is_tanimoto_similar(query_bytes bytea, mol_bytes bytea, threshold float4)
RETURNS boolean
AS 'MODULE_PATHNAME', 'is_tanimoto_similar'
LANGUAGE C STRICT;


create or replace function smiles_to_fingerprint(smiles text)
returns bytea
language plpython3u
as $function$
import chython
import numpy as np
import struct
try:
    query_mol = chython.smiles(smiles)
    morgan = query_mol.morgan_fingerprint(length=2048, max_radius=3)
    bytes_morgan = np.packbits(morgan, bitorder='little').tobytes(order='C')
    popcount = morgan.sum()
    return struct.pack('<h256s', popcount, bytes_morgan)
except Exception as e:
    return None
$function$;
